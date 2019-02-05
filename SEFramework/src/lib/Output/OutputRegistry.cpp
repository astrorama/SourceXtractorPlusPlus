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
    auto matching_properties = m_optional_properties.equal_range(optional);
    for (auto i = matching_properties.first; i != matching_properties.second; ++i) {
      if (std::find(m_output_properties.begin(), m_output_properties.end(), i->second) == m_output_properties.end()) {
        m_output_properties.emplace_back(i->second);
      }
    }
  }
  return [this](const SourceInterface& source) {
    std::vector<ColumnInfo::info_type> info_list {};
    std::vector<Row::cell_type> cell_values {};
    for (const auto& property : m_output_properties) {
      for (const auto& name : m_property_to_names_map.at(property)) {
        auto& col_info = m_name_to_col_info_map.at(name);
        info_list.emplace_back(name, m_name_to_converter_map.at(name).first,
                               col_info.unit, col_info.description);
        cell_values.emplace_back(m_name_to_converter_map.at(name).second(source));
      }
    }
    return Row {std::move(cell_values), std::make_shared<ColumnInfo>(move(info_list))};
  };
}


}
