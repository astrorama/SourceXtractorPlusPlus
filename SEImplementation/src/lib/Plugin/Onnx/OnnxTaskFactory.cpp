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
#include <AlexandriaKernel/memory_tools.h>

namespace SourceXtractor {

// There can be only one!
static Ort::Env ORT_ENV;

/**
 * Extract the shape info from the ONNX ValueInfoProto
 * Note that we expect to have a first dimension of 0, corresponding to an arbitrary batch size,
 * and a second dimension corresponding to the channels, for which we only support 1
 */
static std::tuple<int, std::vector<size_t>> getValueTypeAndShape(const onnx::ValueInfoProto& info) {
  if (!info.type().has_tensor_type()) {
    throw Elements::Exception() << "Only input/output tensors are supported for ONNX models";
  }
  const auto& tensor = info.type().tensor_type();

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

  return std::make_tuple(tensor.elem_type(), shape);
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
    OnnxModel model_info;
    model_info.m_model_path = model_path;

    onnx_logger.info() << "Loading ONNX model " << model_path;

    onnx::ModelProto model_proto;
    std::ifstream in_stream(model_path);
    if (!model_proto.ParseFromIstream(&in_stream)) {
      throw Elements::Exception() << "Could not parse the ONNX model";
    }

    model_info.m_prop_name = model_proto.domain();
    if (!model_info.m_prop_name.empty()) {
      model_info.m_prop_name.push_back('_');
    }
    model_info.m_prop_name += model_proto.graph().name();
    onnx_logger.info() << "Output name will be " << model_info.m_prop_name;

    auto& input_list = model_proto.graph().input();
    if (input_list.empty()) {
      throw Elements::Exception() << "No input on the ONNX model";
    }
    auto& output_list = model_proto.graph().output();
    if (output_list.size() != 1) {
      throw Elements::Exception() << "Only support ONNX models with a single tensor output";
    }

    const auto& input = *input_list.begin();
    const auto& output = *output_list.begin();

    model_info.m_input_name = input.name();
    model_info.m_output_name = output.name();

    std::tie(model_info.m_input_type, model_info.m_input_shape) = getValueTypeAndShape(input);
    std::tie(model_info.m_output_type, model_info.m_output_shape) = getValueTypeAndShape(output);

    if (model_info.m_input_type != onnx::TensorProto::DataType::TensorProto_DataType_FLOAT) {
      throw Elements::Exception() << "Only supports ONNX models with float as input";
    }

    onnx_logger.info() << "ONNX model with input of " << formatShape(model_info.m_input_shape);
    onnx_logger.info() << "ONNX model with output of " << formatShape(model_info.m_output_shape);

    model_info.m_session = Euclid::make_unique<Ort::Session>(ORT_ENV, model_path.c_str(), Ort::SessionOptions{nullptr});
    m_models.emplace_back(std::move(model_info));
  }
}

template<typename T>
static void registerColumnConverter(OutputRegistry& registry, const OnnxModel& model) {
  auto key = model.m_prop_name;

  registry.registerColumnConverter<OnnxProperty, Euclid::NdArray::NdArray<T>>(
    model.m_prop_name, [key](const OnnxProperty& prop) {
      return prop.getData<T>(key);
    }, "", model.m_model_path
  );
}

void OnnxTaskFactory::registerPropertyInstances(OutputRegistry& registry) {
  for (const auto& model : m_models) {
    switch (model.m_output_type) {
      case onnx::TensorProto::DataType::TensorProto_DataType_FLOAT:
        registerColumnConverter<float>(registry, model);
        break;
      case onnx::TensorProto::DataType::TensorProto_DataType_INT32:
        registerColumnConverter<int32_t>(registry, model);
        break;
      default:
        throw Elements::Exception() << "Unsupported output type: "
                                    << onnx::TensorProto_DataType_Name(model.m_output_type);
    }
  }
}

} // end of namespace SourceXtractor
