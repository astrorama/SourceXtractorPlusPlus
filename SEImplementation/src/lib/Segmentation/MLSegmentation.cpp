/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <memory>
#include <vector>
#include <list>
#include <iostream>

#include <onnxruntime_cxx_api.h>

#include "SEUtils/PixelCoordinate.h"
#include "SEUtils/HilbertCurve.h"

#include "SEFramework/Frame/Frame.h"
#include "SEFramework/Image/TileManager.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Common/OnnxCommon.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Property/SourceId.h"

#include "SEImplementation/Measurement/MultithreadedMeasurement.h"


#include "SEImplementation/Grouping/LineSelectionCriteria.h"
#include "SEImplementation/Segmentation/Lutz.h"

#include "SEImplementation/Segmentation/MLSegmentation.h"

#include "SEImplementation/CheckImages/CheckImages.h"


namespace SourceXtractor {

namespace {
/**
 * Pretty-print a vector with shape information
 */
static std::string formatShape(const std::vector<int64_t>& shape) {
  std::ostringstream stream;
  for (auto i = shape.begin(); i != shape.end() - 1; ++i) {
    stream << *i << " x ";
  }
  stream << shape.back();
  return stream.str();
}

class LutzLabellingListener : public Lutz::LutzListener {
public:
  LutzLabellingListener(Segmentation::LabellingListener& listener, std::shared_ptr<SourceFactory> source_factory,
      int window_size) :
    m_listener(listener),
    m_source_factory(source_factory),
    m_window_size(window_size) {}

  virtual ~LutzLabellingListener() = default;

  void publishGroup(Lutz::PixelGroup& pixel_group) override {
    auto source = m_source_factory->createSource();
    source->setProperty<PixelCoordinateList>(pixel_group.pixel_list);
    source->setProperty<SourceId>();
    m_listener.publishSource(source);
  }

  void notifyProgress(int line, int total) override {
    m_listener.notifyProgress(line, total);

    if (m_window_size > 0 && line > m_window_size) {
      m_listener.requestProcessing(
        ProcessSourcesEvent(std::make_shared<LineSelectionCriteria>(line - m_window_size))
      );
    }
  }

private:
  Segmentation::LabellingListener& m_listener;
  std::shared_ptr<SourceFactory> m_source_factory;
  int m_window_size;
};


class OnnxModel {
public:

  OnnxModel(const std::string& model_path);

  template<typename T, typename U>
  void run(std::vector<T>& input_data, std::vector<T>& output_data) const {
    Ort::RunOptions run_options;
    auto mem_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);

    // Allocate memory
    std::vector<int64_t> input_shape(m_input_shape.begin(), m_input_shape.end());
    input_shape[0] = 1;
    size_t input_size = std::accumulate(input_shape.begin(), input_shape.end(), 1u, std::multiplies<size_t>());

    std::vector<int64_t> output_shape(m_output_shape.begin(), m_output_shape.end());
    output_shape[0] = 1;
    size_t output_size = std::accumulate(output_shape.begin(), output_shape.end(), 1u, std::multiplies<size_t>());

    // Setup input/output tensors
    auto input_tensor = Ort::Value::CreateTensor<T>(
      mem_info, input_data.data(), input_data.size(), input_shape.data(), input_shape.size());
    auto output_tensor = Ort::Value::CreateTensor<U>(
      mem_info, output_data.data(), output_data.size(), output_shape.data(), output_shape.size());

    // Run the model
    const char *input_name = m_input_name.c_str();
    const char *output_name = m_output_name.c_str();

    m_session->Run(run_options, &input_name, &input_tensor, 1, &output_name, &output_tensor, 1);
  }

  ONNXTensorElementDataType getInputType() const {
    return m_input_type;
  }

  ONNXTensorElementDataType getOutputType() const {
    return m_output_type;
  }

private:
  std::string m_input_name;  ///< Input tensor name
  std::string m_output_name; ///< Output tensor name
  ONNXTensorElementDataType m_input_type;  ///< Input type
  ONNXTensorElementDataType m_output_type; ///< Output type
  std::vector<std::int64_t> m_input_shape;  ///< Input tensor shape
  std::vector<std::int64_t> m_output_shape; ///< Output tensor shape
  std::string m_model_path; ///< Path to the ONNX model
  std::unique_ptr<Ort::Session> m_session; ///< Session, one per model. In theory, it is thread-safe
};

OnnxModel::OnnxModel(const std::string& model_path) {
  m_model_path = model_path;

  Elements::Logging onnx_logger = Elements::Logging::getLogger("Onnx");
  auto allocator = Ort::AllocatorWithDefaultOptions();

  onnx_logger.info() << "Loading ONNX model " << model_path;
  m_session = Euclid::make_unique<Ort::Session>(ORT_ENV, model_path.c_str(), Ort::SessionOptions{nullptr});

  if (m_session->GetInputCount() != 1) {
    throw Elements::Exception() << "Only ONNX models with a single input tensor are supported";
  }
  if (m_session->GetOutputCount() != 1) {
    throw Elements::Exception() << "Only ONNX models with a single output tensor are supported";
  }

  m_input_name = m_session->GetInputName(0, allocator);
  m_output_name = m_session->GetOutputName(0, allocator);

  auto input_type = m_session->GetInputTypeInfo(0);
  auto output_type = m_session->GetOutputTypeInfo(0);

  m_input_shape = input_type.GetTensorTypeAndShapeInfo().GetShape();
  m_input_type = input_type.GetTensorTypeAndShapeInfo().GetElementType();
  m_output_shape = output_type.GetTensorTypeAndShapeInfo().GetShape();
  m_output_type = output_type.GetTensorTypeAndShapeInfo().GetElementType();

  onnx_logger.info() << "ONNX model with input of " << formatShape(m_input_shape);
  onnx_logger.info() << "ONNX model with output of " << formatShape(m_output_shape);
}

}

void MLSegmentation::labelImage(Segmentation::LabellingListener& listener, std::shared_ptr<const DetectionImageFrame> frame) {
  auto allocator = Ort::AllocatorWithDefaultOptions();

  size_t tile_size = 400;
  m_model_path = "maxidetect-1.0.onnx";

//  size_t tile_size = 400;
//  m_model_path = "test.onnx";

//  size_t tile_size = 672;
//  m_model_path = "super-resolution-10.onnx";

  Elements::Logging onnx_logger = Elements::Logging::getLogger("Onnx");

  OnnxModel model(m_model_path);

  if (model.getInputType() != ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT) {
    throw Elements::Exception() << "Only ONNX models with float input are supported";
  }

  if (model.getOutputType() != ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT) {
    throw Elements::Exception() << "Only ONNX models with float output are supported";
  }

  // allocate memory
  std::vector<float> input_data(tile_size * tile_size);
  std::vector<float> output_data(tile_size * tile_size * 3);

  auto image = frame->getSubtractedImage();
  //auto var_image = frame->getVarianceMap();
  auto check = CheckImages::getInstance().getMoffatImage();
  auto processed_image = VectorImage<float>::create(tile_size, tile_size);

  Lutz lutz;
  LutzLabellingListener lutz_listener(listener, m_source_factory, 0);

  for (int ox=0; ox<image->getWidth(); ox += tile_size) {
    for (int oy=0; oy<image->getHeight(); oy += tile_size) {

      for (int x=0; x<tile_size; x++) {
        for (int y=0; y<tile_size; y++) {
          if (ox+x < image->getWidth() && oy+y < image->getHeight()) {
            input_data[x+y*tile_size] = image->getValue(ox+x, oy+y) / 20.1686; // FIXME get average RMS
          } else {
            input_data[x+y*tile_size] = 0;
          }
        }
      }

      model.run<float, float>(input_data, output_data);

      for (int x=0; x<tile_size; x++) {
        for (int y=0; y<tile_size; y++) {
          if (ox+x < image->getWidth() && oy+y < image->getHeight()) {
//            float a = output_data[(x+y*tile_size) * 3 + 0];
            float b = output_data[(x+y*tile_size) * 3 + 1];
////            float c = output_data[(x+y*tile_size) * 3 + 2];
//            //std::cout << a << ": " << b << " " << c <<"\n";
            processed_image->setValue(x, y,  b - 0.7f);

//            float a = output_data[(x+y*tile_size)];
//            processed_image->setValue(x, y,  b);
            if (check != nullptr) {
              check->setValue(ox+x, oy+y, b);
            }
          } else {
            processed_image->setValue(x, y,  0);
          }
        }
      }

      lutz.labelImage(lutz_listener, *processed_image, PixelCoordinate(ox, oy));
    }
  }
}

}
