/* 
 * File:   OutputRegistry.cpp
 * Author: nikoapos
 *
 * Created on July 28, 2016, 7:12 PM
 */

#include "SEFramework/Registration/OutputRegistry.h"

using namespace Euclid::Table;

namespace SExtractor {

OutputRegistry output_registry;

Euclid::Table::Row OutputRegistry::convertSourceToRow(const SourceInterface& source) {
  std::vector<ColumnInfo::info_type> info_list {};
  std::vector<Row::cell_type> cell_values {};
  for (auto& property : m_output_properties) {
    for (auto& name : m_property_to_names_map[property]) {
      info_list.emplace_back(name, m_name_to_converter_map.at(name).first);
      cell_values.emplace_back(m_name_to_converter_map.at(name).second(source));
    }
  }
  return Row {std::move(cell_values), std::make_shared<ColumnInfo>(move(info_list))};
  
}


}