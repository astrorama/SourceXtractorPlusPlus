/** Copyright © 2020 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include "SEImplementation/Plugin/Onnx/OnnxSourceTask.h"
#include "SEImplementation/Plugin/Onnx/OnnxProperty.h"
#include "SEImplementation/Plugin/DetectionFrameImages/DetectionFrameImages.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include <NdArray/NdArray.h>
#include <AlexandriaKernel/memory_tools.h>
#include <onnxruntime_cxx_api.h>

namespace NdArray = Euclid::NdArray;

namespace SourceXtractor {


template<typename T>
static void fillCutout(const Image<T>& image, int center_x, int center_y, int width, int height, std::vector<T>& out) {
  int x_start = center_x - width / 2;
  int y_start = center_y - height / 2;
  int x_end = x_start + width;
  int y_end = y_start + height;

  ImageAccessor<T> accessor(image);

  int index = 0;
  for (int iy = y_start; iy < y_end; iy++) {
    for (int ix = x_start; ix < x_end; ix++, index++) {
      if (ix >= 0 && iy >= 0 && ix < image.getWidth() && iy < image.getHeight()) {
        out[index] = accessor.getValue(ix, iy);
      }
    }
  }
}

OnnxSourceTask::OnnxSourceTask(const std::vector<OnnxModelInfo>& model_infos) : m_model_infos(model_infos) {}

/**
 * Templated implementation of computeProperties
 * @details
 *  An ONNX model can have different input and output element types (float, integer) with different
 *  precision. We only support float for input, but in order to support also integer outputs
 *  (i.e for classification) we template the computeProperties method on the output value type
 */
template<typename O>
static std::unique_ptr<OnnxProperty::NdWrapperBase>
computePropertiesSpecialized(const OnnxModel& model, const DetectionFrameImages& detection_frame_images,
                             const PixelCentroid& centroid) {
  Ort::RunOptions run_options;
  auto mem_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);

  const int center_x = static_cast<int>(centroid.getCentroidX() + 0.5);
  const int center_y = static_cast<int>(centroid.getCentroidY() + 0.5);

  // Allocate memory
  std::vector<int64_t> input_shape(model.getInputShape().begin(), model.getInputShape().end());
  input_shape[0] = 1;
  size_t input_size = std::accumulate(input_shape.begin(), input_shape.end(), 1u, std::multiplies<size_t>());
  std::vector<float> input_data(input_size);

  std::vector<int64_t> output_shape(model.getOutputShape().begin(), model.getOutputShape().end());
  output_shape[0] = 1;
  size_t output_size = std::accumulate(output_shape.begin(), output_shape.end(), 1u, std::multiplies<size_t>());
  std::vector<O> output_data(output_size);

  // Cut the needed area
  {
    const auto& image = detection_frame_images.getLockedImage(LayerSubtractedImage);
    fillCutout(*image, center_x, center_y, input_shape[2], input_shape[3], input_data);
  }

  model.run<float, O>(input_data, output_data);

  // Set the output
  std::vector<size_t> catalog_shape{model.getOutputShape().begin() + 1, model.getOutputShape().end()};
  return Euclid::make_unique<OnnxProperty::NdWrapper<O>>(catalog_shape, output_data);
}

void OnnxSourceTask::computeProperties(SourceXtractor::SourceInterface& source) const {
  const auto& detection_frame_images = source.getProperty<DetectionFrameImages>();
  const auto& centroid = source.getProperty<PixelCentroid>();

  std::map<std::string, std::unique_ptr<OnnxProperty::NdWrapperBase>> output_dict;

  for (const auto& model_info : m_model_infos) {
    std::unique_ptr<OnnxProperty::NdWrapperBase> result;

    switch (model_info.model->getOutputType()) {
      case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:
        result = computePropertiesSpecialized<float>(*model_info.model, detection_frame_images, centroid);
        break;
      case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:
        result = computePropertiesSpecialized<int32_t>(*model_info.model, detection_frame_images, centroid);
        break;
      default:
        throw Elements::Exception() << "This should have not happened!" << model_info.model->getOutputType();
    }

    output_dict.emplace(model_info.prop_name, std::move(result));
  }

  source.setProperty<OnnxProperty>(std::move(output_dict));
}

} // end of namespace SourceXtractor
