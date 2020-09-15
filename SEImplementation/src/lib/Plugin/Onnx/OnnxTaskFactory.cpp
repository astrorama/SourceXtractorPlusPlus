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
#include <NdArray/NdArray.h>
#include <AlexandriaKernel/memory_tools.h>
#include <onnxruntime_cxx_api.h>

namespace SourceXtractor {

// There can be only one!
static Ort::Env ORT_ENV;

/**
 * Generate a property name based on domain, graph name and output name
 */
static std::string generatePropertyName(const OnnxModel& model_info, OrtAllocator* allocator) {
  std::stringstream prop_name;

  std::string domain = model_info.m_session->GetModelMetadata().GetDomain(allocator);
  if (!domain.empty()) {
    prop_name << domain << '.';
  }

  std::string graph_name = model_info.m_session->GetModelMetadata().GetGraphName(allocator);
  if (!graph_name.empty()) {
    prop_name << graph_name << '.';
  }

  prop_name << model_info.m_output_name;

  return prop_name.str();
}

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
  auto allocator = Ort::AllocatorWithDefaultOptions();

  const auto& onnx_config = manager.getConfiguration<OnnxConfig>();
  const auto& models = onnx_config.getModels();

  for (auto model_path : models) {
    OnnxModel model_info;
    model_info.m_model_path = model_path;

    onnx_logger.info() << "Loading ONNX model " << model_path;
    model_info.m_session = Euclid::make_unique<Ort::Session>(ORT_ENV, model_path.c_str(), Ort::SessionOptions{nullptr});

    if (model_info.m_session->GetInputCount() != 1) {
      throw Elements::Exception() << "Only ONNX models with a single input tensor are supported";
    }
    if (model_info.m_session->GetOutputCount() != 1) {
      throw Elements::Exception() << "Only ONNX models with a single output tensor are supported";
    }

    model_info.m_input_name = model_info.m_session->GetInputName(0, allocator);
    model_info.m_output_name = model_info.m_session->GetOutputName(0, allocator);

    model_info.m_prop_name = generatePropertyName(model_info, allocator);

    onnx_logger.info() << "Output name will be " << model_info.m_prop_name;

    auto input_type = model_info.m_session->GetInputTypeInfo(0);
    auto output_type = model_info.m_session->GetOutputTypeInfo(0);

    model_info.m_input_shape = input_type.GetTensorTypeAndShapeInfo().GetShape();
    model_info.m_input_type = input_type.GetTensorTypeAndShapeInfo().GetElementType();
    model_info.m_output_shape = output_type.GetTensorTypeAndShapeInfo().GetShape();
    model_info.m_output_type = output_type.GetTensorTypeAndShapeInfo().GetElementType();

    if (model_info.m_input_type != ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT) {
      throw Elements::Exception() << "Only ONNX models with float input are supported";
    }
    if (model_info.m_input_shape.size() != 4) {
      throw Elements::Exception() << "Expected 4 axes for the input layer, got " << model_info.m_input_shape.size();
    }

    onnx_logger.info() << "ONNX model with input of " << formatShape(model_info.m_input_shape);
    onnx_logger.info() << "ONNX model with output of " << formatShape(model_info.m_output_shape);

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
      case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:
        registerColumnConverter<float>(registry, model);
        break;
      case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:
        registerColumnConverter<int32_t>(registry, model);
        break;
      default:
        throw Elements::Exception() << "Unsupported output type: " << model.m_output_type;
    }
  }
}

} // end of namespace SourceXtractor
