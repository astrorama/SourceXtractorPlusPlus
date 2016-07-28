/*
 * TextOutput.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: mschefer
 */

#include <iostream>
#include <iomanip>

#include "SEImplementation/Output/TextOutput.h"
#include "SEFramework/Registration/OutputRegistry.h"

namespace SExtractor {

TextOutput::TextOutput(std::ostream& output_stream)
    : m_output_stream(output_stream) {
}

void TextOutput::outputSource(const SourceInterface& source) {
  auto row = output_registry.convertSourceToRow(source);
  for (auto& cell : row) {
    m_output_stream << cell << ' ';
  }
  m_output_stream << std::endl;
}

} // SExtractor namespace
