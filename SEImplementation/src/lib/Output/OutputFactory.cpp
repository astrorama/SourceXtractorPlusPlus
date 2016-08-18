/**
 * @file src/lib/Output/OutputFactory.cpp
 * @date 06/23/16
 * @author mschefer
 */

#include <iostream>
#include <CCfits/CCfits>

#include "Table/AsciiWriter.h"
#include "Table/FitsWriter.h"

#include "SEFramework/Output/OutputRegistry.h"

#include "SEImplementation/Output/OutputFactory.h"
#include "SEImplementation/Output/TableOutput.h"
#include "SEImplementation/Configuration/OutputConfig.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"

namespace SExtractor {

std::unique_ptr<Output> OutputFactory::getOutput() const {
  auto source_to_row = m_output_registry->getSourceToRowConverter(m_optional_properties);
  return std::unique_ptr<Output>(new TableOutput(source_to_row, m_table_hadler));
}

void OutputFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<OutputConfig>();
}

void OutputFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& output_config = manager.getConfiguration<OutputConfig>();
  m_optional_properties = output_config.getOptionalProperties();
  
  auto out_file = output_config.getOutputFile();
  if (out_file != "") {
    m_table_hadler = [out_file](const Euclid::Table::Table& table) {
      CCfits::FITS fits {"!"+out_file, CCfits::RWmode::Write};
      Euclid::Table::FitsWriter{Euclid::Table::FitsWriter::Format::BINARY}.write(fits, "CATALOG", table);
    };
  } else {
    m_table_hadler = [](const Euclid::Table::Table& table) {
      Euclid::Table::AsciiWriter{}.write(std::cout, table);
    };
  }
}

} // SExtractor namespace

