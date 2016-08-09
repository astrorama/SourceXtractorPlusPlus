/* 
 * @file OutputRegistry.h
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_OUTPUTREGISTRY_H
#define _SEFRAMEWORK_OUTPUTREGISTRY_H

#include <functional>
#include <vector>
#include <string>
#include <map>
#include <typeindex>

#include "Table/Row.h"
#include "SEFramework/Source/SourceInterface.h"

namespace SExtractor {

class OutputRegistry {
  
public:
  
  template <typename PropertyType, typename OutType>
  using ColumnConverter = std::function<OutType(const PropertyType&)>;
  
  using SourceToRowConverter = std::function<Euclid::Table::Row(const SourceInterface&)>;
  
  template <typename PropertyType, typename OutType>
  void registerColumnConverter(std::string column_name, ColumnConverter<PropertyType, OutType> converter) {
    m_property_to_names_map[typeid(PropertyType)].emplace_back(column_name);
    std::type_index conv_out_type = typeid(OutType);
    ColumnFromSource conv_func {converter};
    m_name_to_converter_map.emplace(column_name,
                                    std::pair<std::type_index, ColumnFromSource>(conv_out_type, conv_func));
  }
  
  template <typename PropertyType>
  void registerPropertyInstances(const std::vector<std::string>& instance_names) {
    std::vector<std::string> new_names {};
    for (auto& current_name : m_property_to_names_map[typeid(PropertyType)]) {
      // Get the current converter
      auto converter = m_name_to_converter_map.at(current_name);
      for (std::size_t i=0; i<instance_names.size(); ++i) {
        // Make a copy of the converter and set the index
        auto new_converter = converter;
        new_converter.second.index = i;
        // Register the new converter with the new name
        auto& postfix = instance_names[i];
        auto new_name = current_name + "_" + postfix;
        m_name_to_converter_map.emplace(new_name, new_converter);
        new_names.push_back(new_name);
      }
      // Remove the old converter
      m_name_to_converter_map.erase(current_name);
    }
    // Replace all the old names with the new ones
    m_property_to_names_map[typeid(PropertyType)] = new_names;
  }
  
  template <typename PropertyType>
  void enableOutput() {
    if (m_property_to_names_map.count(typeid(PropertyType)) == 0) {
      throw Elements::Exception() << "No registered ColumnConverters for"
              << " property " << typeid(PropertyType).name();
    }
    m_output_properties.emplace_back(typeid(PropertyType));
  }
  
  SourceToRowConverter getSourceToRowConverter();
  
private:
  
  class ColumnFromSource {
  public:
    template <typename PropertyType, typename OutType>
    ColumnFromSource(ColumnConverter<PropertyType, OutType> converter) {
      m_convert_func = [converter](const SourceInterface& source, std::size_t index){
        return converter(source.getProperty<PropertyType>(index));
      };
    }
    Euclid::Table::Row::cell_type operator()(const SourceInterface& source) {
      return m_convert_func(source, index);
    }
    std::size_t index = 0;
  private:
    std::function<Euclid::Table::Row::cell_type(const SourceInterface&, std::size_t index)> m_convert_func;
  };
  
  std::map<std::type_index, std::vector<std::string>> m_property_to_names_map {};
  std::map<std::string, std::pair<std::type_index, ColumnFromSource>> m_name_to_converter_map {};
  std::vector<std::type_index> m_output_properties {};
  
};

} /* namespace SExtractor */

#endif /* _SEFRAMEWORK_OUTPUTREGISTRY_H */

