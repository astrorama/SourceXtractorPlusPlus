/**
 * @file src/lib/Output/OutputFactory.cpp
 * @date 06/23/16
 * @author mschefer
 */

#include <iostream>

#include "Table/AsciiWriter.h"

#include "SEFramework/Registration/OutputRegistry.h"

#include "SEImplementation/Output/OutputFactory.h"
#include "SEImplementation/Output/TableOutput.h"
#include "SEImplementation/Configuration/OutputConfig.h"

#include "SEImplementation/Property/PixelCentroid.h"

namespace SExtractor {

std::unique_ptr<Output> OutputFactory::getOutput() const {
  auto source_to_row = output_registry.getSourceToRowConverter();
  auto std_out_handler = [](const Euclid::Table::Table& table) {
    Euclid::Table::AsciiWriter{}.write(std::cout, table);
  };
  return std::unique_ptr<Output>(new TableOutput(source_to_row, std_out_handler));
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
  
  output_config.setEnabledOutputs(output_registry);
}

} // SExtractor namespace

