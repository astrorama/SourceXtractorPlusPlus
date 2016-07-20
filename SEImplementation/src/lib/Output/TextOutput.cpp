/*
 * TextOutput.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: mschefer
 */

#include <iostream>
#include <iomanip>

#include "SEFramework/Output/OutputColumn.h"
#include "SEImplementation/Output/TextOutput.h"

namespace SExtractor {

TextOutput::TextOutput(std::vector<OutputColumn> output_columns, std::ostream& output_stream)
    : m_output_columns(std::move(output_columns)),
      m_output_stream(output_stream) {
}

void TextOutput::outputSource(const SourceInterface& source) {
  for (auto& column : m_output_columns) {
    auto any_value = column.getValue(source);
    if (any_value.type() == typeid(double)) {
      auto double_value = boost::any_cast<double>(column.getValue(source));
      m_output_stream << double_value << " ";
    } else if (any_value.type() == typeid(int)) {
      auto int_value = boost::any_cast<int>(column.getValue(source));
      m_output_stream << int_value << " ";
    } else if (any_value.type() == typeid(std::int64_t)) {
      auto int_value = boost::any_cast<std::int64_t>(any_value);
      m_output_stream << int_value << " ";
    }
  }
  m_output_stream << std::endl;
}

} // SExtractor namespace
