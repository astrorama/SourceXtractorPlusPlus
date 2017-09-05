/**
 * @file src/lib/Output/OutputFactory.cpp
 * @date 06/23/16
 * @author mschefer
 */

#include <iostream>
#include <fstream>
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
  return std::unique_ptr<Output>(new TableOutput(source_to_row, m_table_handler));
}

void OutputFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<OutputConfig>();
}

void OutputFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& output_config = manager.getConfiguration<OutputConfig>();
  m_optional_properties = output_config.getOptionalProperties();
  
  auto out_file = output_config.getOutputFile();
  if (out_file != "") {
    switch (output_config.getOutputFileFormat()) {
      case OutputConfig::OutputFileFormat::FITS:
        m_table_handler = [out_file](const Euclid::Table::Table& table) {
          Euclid::Table::FitsWriter{out_file}.setHduName("CATALOG").addData(table);
        };
        break;
      case OutputConfig::OutputFileFormat::ASCII:
        m_table_handler = [out_file](const Euclid::Table::Table& table) {
          std::ofstream out_stream {out_file};
          Euclid::Table::AsciiWriter{out_stream}.addData(table);
        };
        break;
    }
  } else {
    m_table_handler = [](const Euclid::Table::Table& table) {
      Euclid::Table::AsciiWriter{std::cout}.addData(table);
    };
  }
}

} // SExtractor namespace

