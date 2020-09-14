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
#include <NdArray/NdArray.h>
#include <onnxruntime_cxx_api.h>
#include <SEImplementation/Plugin/DetectionFrameImages/DetectionFrameImages.h>
#include <SEImplementation/Plugin/PixelCentroid/PixelCentroid.h>

namespace NdArray = Euclid::NdArray;

namespace SourceXtractor {

// There can be only one!
static Ort::Env ORT_ENV;

template<typename T>
static void fillCutout(const Image<T>& image, int center_x, int center_y, int width, int height, std::vector<T>& out) {
  int x_start = center_x - width / 2;
  int y_start = center_y - height / 2;
  int x_end = x_start + width;
  int y_end = y_start + height;

  int index = 0;
  for (int iy = y_start; iy < y_end; iy++) {
    for (int ix = x_start; ix < x_end; ix++, index++) {
      if (ix >= 0 && iy >= 0 && ix < image.getWidth() && iy < image.getHeight()) {
        out[index] = image.getValue(ix, iy);
      }
    }
  }
}

OnnxSourceTask::OnnxSourceTask(const std::vector<OnnxModel>& models) : m_models(models) {}

void OnnxSourceTask::computeProperties(SourceXtractor::SourceInterface& source) const {
  /*
   * Prepare the image
   */
  const auto& detection_frame_images = source.getProperty<DetectionFrameImages>();

  const auto& centroid = source.getProperty<PixelCentroid>();
  const int center_x = static_cast<int>(centroid.getCentroidX() + 0.5);
  const int center_y = static_cast<int>(centroid.getCentroidY() + 0.5);

  /*
   * Output
   */
  std::map<std::string, NdArray::NdArray<float>> output_dict;

  Ort::RunOptions run_options;
  auto mem_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);

  for (const auto& model : m_models) {
    Ort::Session session{ORT_ENV, model.m_model_path.c_str(), Ort::SessionOptions{nullptr}};

    // Allocate memory
    std::vector<int64_t> input_shape(model.m_input_shape.begin(), model.m_input_shape.end());
    input_shape[0] = 1;
    size_t input_size = std::accumulate(input_shape.begin(), input_shape.end(), 1u, std::multiplies<size_t>());
    std::vector<float> input_data(input_size);

    std::vector<int64_t> output_shape(model.m_output_shape.begin(), model.m_output_shape.end());
    output_shape[0] = 1;
    size_t output_size = std::accumulate(output_shape.begin(), output_shape.end(), 1u, std::multiplies<size_t>());
    std::vector<float> output_data(output_size);

    // Cut the needed area
    {
      const auto& image = detection_frame_images.getLockedImage(LayerSubtractedImage);
      fillCutout(*image, center_x, center_y, input_shape[2], input_shape[3], input_data);
    }

    // Setup input/output tensors
    auto input_tensor = Ort::Value::CreateTensor<float>(
      mem_info, input_data.data(), input_data.size(), input_shape.data(), input_shape.size());
    auto output_tensor = Ort::Value::CreateTensor<float>(
      mem_info, output_data.data(), output_data.size(), output_shape.data(), output_shape.size());

    // Run the model
    const char *input_name = model.m_input_name.c_str();
    const char *output_name = model.m_output_name.c_str();
    session.Run(run_options,
                &input_name, &input_tensor, 1,
                &output_name, &output_tensor, 1);

    // Set the output
    std::vector<size_t> catalog_shape{model.m_output_shape.begin() + 1, model.m_output_shape.end()};
    output_dict.emplace(model.m_model_path, NdArray::NdArray<float>(catalog_shape, output_data));
  }

  source.setProperty<OnnxProperty>(std::move(output_dict));
}

} // end of namespace SourceXtractor
