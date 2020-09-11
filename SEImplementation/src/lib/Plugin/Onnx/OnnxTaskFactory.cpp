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

#include "SEImplementation/Plugin/Onnx/OnnxPlugin.h"
#include "SEImplementation/Plugin/Onnx/OnnxTaskFactory.h"
#include "SEImplementation/Plugin/Onnx/OnnxSourceTask.h"
#include "SEImplementation/Plugin/Onnx/OnnxProperty.h"
#include "SEImplementation/Plugin/Onnx/OnnxConfig.h"
#include "onnx.pb.h"
#include <NdArray/NdArray.h>

namespace SourceXtractor {

/**
 * Extract the shape info from the ONNX ValueInfoProto
 * Note that we expect to have a first dimension of 0, corresponding to an arbitrary batch size,
 * and a second dimension corresponding to the channels, for which we only support 1
 */
static std::vector<size_t> getValueShape(const onnx::ValueInfoProto& info) {
  if (!info.type().has_tensor_type()) {
    throw Elements::Exception() << "Only input/output tensors are supported for ONNX models";
  }
  const auto& tensor = info.type().tensor_type();
  if (tensor.elem_type() != onnx::TensorProto::DataType::TensorProto_DataType_FLOAT) {
    throw Elements::Exception() << "Only input/output float tensors are supported for ONNX models";
  }

  std::vector<size_t> shape;
  std::transform(tensor.shape().dim().begin(), tensor.shape().dim().end(), std::back_inserter(shape),
                 [](onnx::TensorShapeProto::Dimension d) { return d.dim_value(); });

  if (shape.size() != 4) {
    throw Elements::Exception() << "Expected 4 axes";
  }
  if (shape[0] != 0) {
    throw Elements::Exception() << "Expected a batch size of 0";
  }
  if (shape[1] != 1) {
    throw Elements::Exception() << "Only 1 channel supported for ONNX models";
  }
  return shape;
}

std::string formatShape(const std::vector<size_t>& shape) {
  std::ostringstream stream;
  for (auto i = shape.begin(); i != shape.end() - 1; ++i) {
    stream << *i << " x ";
  }
  stream << shape.back();
  return stream.str();
}

OnnxTaskFactory::OnnxTaskFactory() {}

std::shared_ptr<Task> OnnxTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<OnnxProperty>()) {
    return std::make_shared<OnnxSourceTask>(m_models);
  }
  return nullptr;
}

void OnnxTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<OnnxConfig>();
}

void OnnxTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  const auto& onnx_config = manager.getConfiguration<OnnxConfig>();
  const auto& models = onnx_config.getModels();

  for (auto model_path : models) {
    onnx_logger.info() << "Loading ONNX model " << model_path;

    onnx::ModelProto model;
    std::ifstream in_stream(model_path);
    if (!model.ParseFromIstream(&in_stream)) {
      throw Elements::Exception() << "Could not parse the ONNX model";
    }

    std::string output_name = model.domain();
    if (!output_name.empty()) {
      output_name.push_back('_');
    }
    output_name += model.graph().name();
    onnx_logger.info() << "Output name will be " << output_name;

    auto& input_list = model.graph().input();
    if (input_list.empty()) {
      throw Elements::Exception() << "No input on the ONNX model";
    }
    auto& output_list = model.graph().output();
    if (output_list.size() != 1) {
      throw Elements::Exception() << "Only support ONNX models with a single tensor output";
    }

    const auto& input = *input_list.begin();
    const auto& output = *output_list.begin();

    auto input_shape = getValueShape(input);
    auto output_shape = getValueShape(output);

    onnx_logger.info() << "ONNX model with input of " << formatShape(input_shape);
    onnx_logger.info() << "ONNX model with output of " << formatShape(output_shape);

    m_models.emplace_back(OnnxModel{
      output_name, input.name(), output.name(), input_shape, output_shape, model_path
    });
  }
}

void OnnxTaskFactory::registerPropertyInstances(OutputRegistry& registry) {
  for (const auto& model : m_models) {
    auto key = model.m_model_path;
    registry.registerColumnConverter<OnnxProperty, Euclid::NdArray::NdArray<float>>(
      model.m_prop_name, [key](const OnnxProperty& prop) {
        return prop.getData(key);
      }, "", model.m_model_path
    );
  }
}

} // end of namespace SourceXtractor
