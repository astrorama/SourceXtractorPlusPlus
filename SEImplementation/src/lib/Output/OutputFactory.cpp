/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
/**
 * @file src/lib/Output/OutputFactory.cpp
 * @date 06/23/16
 * @author mschefer
 */
#include <system_error>
#include <iostream>
#include <fstream>
#include <system_error>
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
  auto source_to_row = m_output_registry->getSourceToRowConverter(m_output_properties);
  return std::unique_ptr<Output>(new TableOutput(source_to_row, m_table_handler));
}

void OutputFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<OutputConfig>();
}

void OutputFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& output_config = manager.getConfiguration<OutputConfig>();
  m_output_properties = output_config.getOutputProperties();
  
  auto out_file = output_config.getOutputFile();

  if (out_file != "") {
    // Check if we can, at least, create it.
    // Otherwise, the error will be triggered only at the end of the full process!
    {
      std::ofstream check_writeable{out_file};
      if (!check_writeable) {
        throw Elements::Exception(
          std::system_error(errno, std::system_category(), "Failed to open the output catalog").what());
      }
    }

    switch (output_config.getOutputFileFormat()) {
      case OutputConfig::OutputFileFormat::FITS:
        m_table_handler = [out_file](const Euclid::Table::Table& table) {
          try {
            Euclid::Table::FitsWriter{out_file, true}.setHduName("CATALOG").addData(table);
          }
          // This one doesn't inherit from std::exception, so wrap it up here
          catch (const CCfits::FitsException &e) {
            throw Elements::Exception(e.message());
          }
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

