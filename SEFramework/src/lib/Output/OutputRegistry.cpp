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

auto OutputRegistry::getSourceToRowConverter(const std::vector<std::string>& enabled_properties) -> SourceToRowConverter {
  std::vector<std::type_index> out_prop_list {};
  for (auto& prop : enabled_properties) {
    if (m_output_properties.count(prop) == 0) {
      throw Elements::Exception() << "Unknown output property " << prop;
    }
    auto matching_properties = m_output_properties.equal_range(prop);
    for (auto i = matching_properties.first; i != matching_properties.second; ++i) {
      if (std::find(out_prop_list.begin(), out_prop_list.end(), i->second) == out_prop_list.end()) {
        out_prop_list.emplace_back(i->second);
      }
    }
  }
  return [this, out_prop_list](const SourceInterface& source) {
    std::vector<ColumnInfo::info_type> info_list {};
    std::vector<Row::cell_type> cell_values {};
    for (const auto& property : out_prop_list) {
      for (const auto& name : m_property_to_names_map.at(property)) {
        info_list.emplace_back(name, m_name_to_converter_map.at(name).first);
        cell_values.emplace_back(m_name_to_converter_map.at(name).second(source));
      }
    }
    return Row {std::move(cell_values), std::make_shared<ColumnInfo>(move(info_list))};
  };
}


}
