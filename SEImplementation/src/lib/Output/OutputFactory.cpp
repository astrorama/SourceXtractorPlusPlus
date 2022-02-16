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

#include "SEFramework/Output/OutputRegistry.h"

#include "SEImplementation/Configuration/DetectionImageConfig.h"

#include "SEImplementation/Output/AsciiOutput.h"
#include "SEImplementation/Output/FitsOutput.h"
#include "SEImplementation/Output/LdacOutput.h"

#include "SEImplementation/Output/OutputFactory.h"

using Euclid::make_unique;

namespace SourceXtractor {

std::shared_ptr<Output> OutputFactory::createOutput() const {
  auto source_to_row = m_output_registry->getSourceToRowConverter(m_output_properties);

  if (m_output_filename != "") {
    switch (m_output_format) {
      case OutputConfig::OutputFileFormat::FITS:
        return std::make_shared<FitsOutput>(m_output_filename, source_to_row, m_flush_size);
      case OutputConfig::OutputFileFormat::FITS_LDAC:
        return std::make_shared<LdacOutput>(m_output_filename, source_to_row, m_flush_size);
      default:
      case OutputConfig::OutputFileFormat::ASCII:
        return std::make_shared<AsciiOutput>(m_output_filename, source_to_row, m_flush_size);
    }
  } else {
    return std::make_shared<AsciiOutput>(m_output_filename, source_to_row, m_flush_size);
  }
}

void OutputFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<OutputConfig>();
}

void OutputFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto& output_config = manager.getConfiguration<OutputConfig>();
  m_output_properties = output_config.getOutputProperties();
  m_flush_size = output_config.getFlushSize();
  m_output_filename = output_config.getOutputFile();
  m_output_format = output_config.getOutputFileFormat();

  if (m_output_filename != "") {
    // Check if we can, at least, create it.
    // Otherwise, the error will be triggered only at the end of the full process!
    std::ofstream check_writeable{m_output_filename};
    if (!check_writeable) {
      throw Elements::Exception(
        std::system_error(errno, std::system_category(), "Failed to open the output catalog").what());
    }
  }
}

} // SourceXtractor namespace

