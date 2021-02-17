/*
 * OnnxModel.h
 *
 *  Created on: Feb 16, 2021
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_COMMON_ONNXMODEL_H_
#define _SEIMPLEMENTATION_COMMON_ONNXMODEL_H_

#include <memory>
#include <vector>
#include <list>
#include <iostream>
#include <numeric>

#include <onnxruntime_cxx_api.h>

namespace SourceXtractor {

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

}


#endif /* _SEIMPLEMENTATION_COMMON_ONNXMODEL_H_ */
