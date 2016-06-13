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

#include <boost/functional/hash.hpp>

namespace SExtractor {

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
struct hash<SExtractor::PropertyId>
{
  std::size_t operator()(const SExtractor::PropertyId& id) const {
    std::size_t hash = 0;
    boost::hash_combine(hash, id.m_parameter);
    boost::hash_combine(hash, id.m_property_type);
    return hash;
  }
};

}

#endif
