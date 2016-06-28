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

TextOutput::TextOutput(std::vector<OutputColumn> output_columns) : m_output_columns(std::move(output_columns)) {
}

void TextOutput::outputSource(const SourceInterface& source) {
  for (auto& column : m_output_columns) {
    auto any_value = column.getValue(source);
    if (any_value.type() == typeid(double)) {
      auto double_value = boost::any_cast<double>(column.getValue(source));
      std::cout << double_value << " ";
    } else if (any_value.type() == typeid(int)) {
      auto int_value = boost::any_cast<int>(column.getValue(source));
      std::cout << int_value << " ";
    }
  }
  std::cout << std::endl;
}

} // SExtractor namespace
