/**
 * @file src/lib/Output/OutputFactory.cpp
 * @date 06/23/16
 * @author mschefer
 */

#include <iostream>

#include "Table/AsciiWriter.h"

#include "SEFramework/Output/OutputRegistry.h"

#include "SEImplementation/Output/OutputFactory.h"
#include "SEImplementation/Output/TableOutput.h"
#include "SEImplementation/Configuration/OutputConfig.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"

namespace SExtractor {

std::unique_ptr<Output> OutputFactory::getOutput() const {
  auto source_to_row = m_output_registry->getSourceToRowConverter();
  auto std_out_handler = [](const Euclid::Table::Table& table) {
    Euclid::Table::AsciiWriter{}.write(std::cout, table);
  };
  return std::unique_ptr<Output>(new TableOutput(source_to_row, std_out_handler));
}

void OutputFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<OutputConfig>();
}

void OutputFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& output_config = manager.getConfiguration<OutputConfig>();
  output_config.setEnabledOutputs(*m_output_registry);
}

} // SExtractor namespace

