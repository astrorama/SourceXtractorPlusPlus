/*
 * OnnxModel.h
 *
 *  Created on: Feb 16, 2021
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_COMMON_ONNXMODEL_H_
#define _SEIMPLEMENTATION_COMMON_ONNXMODEL_H_

#include <cstdint>
#include <memory>
#include <vector>
#include <list>
#include <iostream>
#include <numeric>

#include <onnxruntime_cxx_api.h>

namespace SourceXtractor {

class OnnxModel {
public:

  explicit OnnxModel(const std::string& model_path);

  template<typename T, typename U>
  void run(std::vector<T>& input_data, std::vector<U>& output_data) const {
    Ort::RunOptions run_options;
    auto mem_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);

    // Allocate memory
    std::vector<int64_t> input_shape(m_input_shapes[0].begin(), m_input_shapes[0].end());
    input_shape[0] = 1;
    size_t input_size = std::accumulate(input_shape.begin(), input_shape.end(), 1u, std::multiplies<size_t>());

    std::vector<int64_t> output_shape(m_output_shape.begin(), m_output_shape.end());
    output_shape[0] = 1;
    size_t output_size = std::accumulate(output_shape.begin(), output_shape.end(), 1u, std::multiplies<size_t>());

    // Check input and output size are OK
    if (input_data.size() < input_size || output_data.size() < output_size) {
      throw Elements::Exception() << "OnnxModel: Insufficient buffer size ";
    }

    // Setup input/output tensors
    auto input_tensor = Ort::Value::CreateTensor<T>(
      mem_info, input_data.data(), input_data.size(), input_shape.data(), input_shape.size());
    auto output_tensor = Ort::Value::CreateTensor<U>(
      mem_info, output_data.data(), output_data.size(), output_shape.data(), output_shape.size());

    // Run the model
    const char *input_name = m_input_names[0].c_str();
    const char *output_name = m_output_name.c_str();

    m_session->Run(run_options, &input_name, &input_tensor, 1, &output_name, &output_tensor, 1);
  }

  template<typename T, typename U>
  void runMultiInput(std::map<std::string, std::vector<T>>& input_data, std::vector<U>& output_data) const {
    Ort::RunOptions run_options;
    auto mem_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);

    std::vector<const char *> input_names;
    std::vector<Ort::Value> input_tensors;

    int inputs_nb = m_input_names.size();
    for (int i=0; i<inputs_nb; i++) {
      input_names.emplace_back(m_input_names[i].c_str());

      // Allocate memory
      std::vector<int64_t> input_shape(m_input_shapes[i].begin(), m_input_shapes[i].end());
      input_shape[0] = 1;
      size_t input_size = std::accumulate(input_shape.begin(), input_shape.end(), 1u, std::multiplies<size_t>());

      // Check input size is OK
      if (input_data[m_input_names[i]].size() < input_size) {
        throw Elements::Exception() << "OnnxModel: Insufficient buffer size ";
      }

      input_tensors.emplace_back(Ort::Value::CreateTensor<T>(
        mem_info, input_data[m_input_names[i]].data(), input_data[m_input_names[i]].size(),
        input_shape.data(), input_shape.size()));
    }

    // Output name and shape
    const char *output_name = m_output_name.c_str();
    std::vector<int64_t> output_shape(m_output_shape.begin(), m_output_shape.end());
    output_shape[0] = 1;

    // Setup output tensor
    size_t output_size = std::accumulate(output_shape.begin(), output_shape.end(), 1u, std::multiplies<size_t>());

    // Check output and output size are OK
    if (output_data.size() < output_size) {
      throw Elements::Exception() << "OnnxModel: Insufficient buffer size ";
    }

    auto output_tensor = Ort::Value::CreateTensor<U>(
      mem_info, output_data.data(), output_data.size(), output_shape.data(), output_shape.size());

    // Run the model
    m_session->Run(run_options, &input_names[0], &input_tensors[0], inputs_nb, &output_name, &output_tensor, 1);
  }


  ONNXTensorElementDataType getInputType() const {
    return m_input_types[0];
  }

  ONNXTensorElementDataType getOutputType() const {
    return m_output_type;
  }

  const std::vector<std::int64_t>& getInputShape() const {
    return m_input_shapes[0];
  }

  const std::vector<std::int64_t>& getOutputShape() const {
    return m_output_shape;
  }

  std::string getDomain() const {
    return m_domain_name;
  }

  std::string getGraphName() const {
    return m_graph_name;
  }

  std::string getInputName() const {
    return m_input_names[0];
  }

  std::string getOutputName() const {
    return m_output_name;
  }

  std::string getModelPath() const {
    return m_model_path;
  }

  size_t getInputNb() const {
    return m_input_names.size();
  }

  size_t getOutputNb() const {
    return 1U;
  }

private:
  std::string m_domain_name;  ///< domain name
  std::string m_graph_name;  ///< graph name
  std::vector<std::string> m_input_names;  ///< Input tensor name
  std::string m_output_name; ///< Output tensor name
  std::vector<ONNXTensorElementDataType> m_input_types;  ///< Input type
  ONNXTensorElementDataType m_output_type; ///< Output type
  std::vector<std::vector<std::int64_t>> m_input_shapes;  ///< Input tensor shape
  std::vector<std::int64_t> m_output_shape; ///< Output tensor shape
  std::string m_model_path; ///< Path to the ONNX model
  std::unique_ptr<Ort::Session> m_session; ///< Session, one per model. In theory, it is thread-safe
};

}


#endif /* _SEIMPLEMENTATION_COMMON_ONNXMODEL_H_ */
