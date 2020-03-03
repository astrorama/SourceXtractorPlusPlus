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

#include "AlexandriaKernel/memory_tools.h"
#include "Table/AsciiWriter.h"
#include "Table/FitsWriter.h"

#include "SEFramework/Output/OutputRegistry.h"

#include "SEImplementation/Output/OutputFactory.h"
#include "SEImplementation/Output/TableOutput.h"
#include "SEImplementation/Configuration/OutputConfig.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"

using Euclid::make_unique;

namespace SourceXtractor {

std::unique_ptr<Output> OutputFactory::getOutput() const {
  auto source_to_row = m_output_registry->getSourceToRowConverter(m_output_properties);
  return std::unique_ptr<Output>(new TableOutput(source_to_row, m_table_handler, m_flush_size));
}

void OutputFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<OutputConfig>();
}

void OutputFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& output_config = manager.getConfiguration<OutputConfig>();
  m_output_properties = output_config.getOutputProperties();
  m_flush_size = output_config.getFlushSize();
  
  auto out_file = output_config.getOutputFile();

  std::shared_ptr<Euclid::Table::TableWriter> table_writer;

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

    std::unique_ptr<Euclid::Table::FitsWriter> fits_table_writer;

    switch (output_config.getOutputFileFormat()) {
      case OutputConfig::OutputFileFormat::FITS:
        fits_table_writer = make_unique<Euclid::Table::FitsWriter>(out_file, true);
        fits_table_writer->setHduName("CATALOG");
        table_writer = std::move(fits_table_writer);
        break;
      case OutputConfig::OutputFileFormat::ASCII:
        table_writer = std::make_shared<Euclid::Table::AsciiWriter>(out_file);
        break;
    }
  } else {
    table_writer = std::make_shared<Euclid::Table::AsciiWriter>(std::cout);
  }

  m_table_handler = [table_writer](const Euclid::Table::Table& table) {
    try {
      table_writer->addData(table);
    }
    // This one doesn't inherit from std::exception, so wrap it up here
    catch (const CCfits::FitsException &e) {
      throw Elements::Exception(e.message());
    }
  };
}

} // SourceXtractor namespace

