/* 
 * File:   OutputRegistry.cpp
 * Author: nikoapos
 *
 * Created on July 28, 2016, 7:12 PM
 */

#include <algorithm>

#include "ElementsKernel/Exception.h"

#include "SEFramework/Output/OutputRegistry.h"

using namespace Euclid::Table;

namespace SExtractor {

auto OutputRegistry::getSourceToRowConverter(const std::vector<std::string>& enabled_optional) -> SourceToRowConverter {
  for (auto& optional : enabled_optional) {
    if (m_optional_properties.count(optional) == 0) {
      throw Elements::Exception() << "Unknown output property " << optional;
    }
    auto type_index = m_optional_properties.at(optional);
    if (std::find(m_output_properties.begin(), m_output_properties.end(), type_index) == m_output_properties.end()) {
      m_output_properties.emplace_back(type_index);
    }
  }
  return [this](const SourceInterface& source) {
    std::vector<ColumnInfo::info_type> info_list {};
    std::vector<Row::cell_type> cell_values {};
    for (const auto& property : m_output_properties) {
      for (const auto& name : m_property_to_names_map.at(property)) {
        info_list.emplace_back(name, m_name_to_converter_map.at(name).first);
        cell_values.emplace_back(m_name_to_converter_map.at(name).second(source));
      }
    }
    return Row {std::move(cell_values), std::make_shared<ColumnInfo>(move(info_list))};
  };
}


}
