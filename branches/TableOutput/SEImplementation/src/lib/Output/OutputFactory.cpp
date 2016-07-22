/**
 * @file src/lib/Output/OutputFactory.cpp
 * @date 06/23/16
 * @author mschefer
 */

#include "SEImplementation/Output/OutputFactory.h"
#include "SEImplementation/Output/TextOutput.h"
#include "SEImplementation/Configuration/OutputConfig.h"

#include "SEImplementation/Property/PixelCentroid.h"

namespace SExtractor {

std::unique_ptr<Output> OutputFactory::getOutput() const {
  return std::unique_ptr<Output>(new TextOutput(m_output_columns, std::cout));
}

void OutputFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) {
  manager.registerConfiguration<OutputConfig>();
}

void OutputFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& output_config = manager.getConfiguration<OutputConfig>();
  auto& output_columns_names = output_config.getOutputColumns();
  if (output_columns_names.size() == 0) {
    throw Elements::Exception() << "No output columns are requested";
  }

  auto& registered_columns = RegistrationManager::instance().getOutputColumns();
  for (auto& column_name : output_columns_names) {
    auto column_iter = registered_columns.find(column_name);
    if (column_iter != registered_columns.end()) {
      m_output_columns.emplace_back(column_iter->second);
    }
  }
}

} // SExtractor namespace

