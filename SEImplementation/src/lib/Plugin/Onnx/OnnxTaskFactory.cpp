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

#include <onnxruntime_cxx_api.h>

#include <AlexandriaKernel/memory_tools.h>
#include <NdArray/NdArray.h>

#include "SEImplementation/Common/OnnxCommon.h"

#include "SEImplementation/Plugin/Onnx/OnnxPlugin.h"
#include "SEImplementation/Plugin/Onnx/OnnxSourceTask.h"
#include "SEImplementation/Plugin/Onnx/OnnxProperty.h"
#include "SEImplementation/Plugin/Onnx/OnnxConfig.h"

#include "SEImplementation/Plugin/Onnx/OnnxTaskFactory.h"

namespace SourceXtractor {

/**
 * Generate a property name based on domain, graph name and output name
 */
static std::string generatePropertyName(const OnnxModel& model) {
  std::stringstream prop_name;

  std::string domain = model.getDomain();
  if (!domain.empty()) {
    prop_name << domain << '.';
  }

  std::string graph_name = model.getGraphName();
  if (!graph_name.empty()) {
    prop_name << graph_name << '.';
  }

  prop_name << model.getOutputName();

  return prop_name.str();
}

OnnxTaskFactory::OnnxTaskFactory() {}

std::shared_ptr<Task> OnnxTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<OnnxProperty>()) {
    return std::make_shared<OnnxSourceTask>(m_model_infos);
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
    auto model = std::make_shared<OnnxModel>(model_path);

    if (model->getInputType() != ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT) {
      throw Elements::Exception() << "Only ONNX models with float input are supported";
    }

    if (model->getInputShape().size() != 4) {
      throw Elements::Exception() << "Expected 4 axes for the input layer, got " << model->getInputShape().size();
    }

    auto prop_name = generatePropertyName(*model);
    onnx_logger.info() << "Output name will be " << prop_name;

    m_model_infos.emplace_back(OnnxSourceTask::OnnxModelInfo {model, prop_name});

  }
}

template<typename T>
static void registerColumnConverter(OutputRegistry& registry, const OnnxSourceTask::OnnxModelInfo& model_info) {
  auto key = model_info.prop_name;

  registry.registerColumnConverter<OnnxProperty, Euclid::NdArray::NdArray<T>>(
      model_info.prop_name, [key](const OnnxProperty& prop) {
      return prop.getData<T>(key);
    }, "", model_info.model->getModelPath()
  );
}

void OnnxTaskFactory::registerPropertyInstances(OutputRegistry& registry) {
  for (const auto& model_info : m_model_infos) {
    switch (model_info.model->getOutputType()) {
      case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:
        registerColumnConverter<float>(registry, model_info);
        break;
      case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:
        registerColumnConverter<int32_t>(registry, model_info);
        break;
      default:
        throw Elements::Exception() << "Unsupported output type: " << model_info.model->getOutputType();
    }
  }
}

} // end of namespace SourceXtractor
