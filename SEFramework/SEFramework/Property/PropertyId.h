/**
 * @file SEFramework/Property/PropertyId.h
 * @date 05/17/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_PROPERTY_PROPERTYID_H
#define _SEFRAMEWORK_PROPERTY_PROPERTYID_H

#include <typeindex>
#include <string>
#include <functional>

namespace SEFramework {

class PropertyId {
public:
  PropertyId(const std::type_info& property_type, const std::string& parameter="") :
    m_property_type (property_type), m_parameter(parameter) {
  }

  /// Comparison operator is needed to be use PropertyId as key in unordered_map
  bool operator==(const PropertyId other) const {
    return m_property_type == other.m_property_type && m_parameter == other.m_parameter;
  }

  std::type_index m_property_type;

  // TODO currently using a string as generic property id parameter but should be a generic way to specify parameters
  std::string m_parameter;
};

}

namespace std {

// defines a hash for PropertyId, this is to be able to use PropertyId as a key in a std::unordered_map

template <>
struct hash<SEFramework::PropertyId>
{
  std::size_t operator()(const SEFramework::PropertyId& id) const {
     return hash<std::string>()(id.m_parameter) ^ hash<std::type_index>()(id.m_property_type);
  }
};

}

#endif
