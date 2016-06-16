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

#include "SEFramework/Registration/RegistrationManager.h"

namespace SExtractor {

class PropertyId {
public:
  template<typename T, class R = RegistrationManager>
  static PropertyId create(unsigned int index = 0) {
    static unsigned int id = R::instance().getNextPropertyId();
    return PropertyId(id, index);
  }

  /// Comparison operator is needed to be use PropertyId as key in unordered_map
  bool operator==(PropertyId other) const {
    return m_type_id == other.m_type_id && m_index == other.m_index;
  }

  bool operator<(PropertyId other) const {
    if (m_type_id == other.m_type_id) {
      return m_index < other.m_index;
    }

    return m_type_id < other.m_type_id;
  }

private:
  PropertyId(unsigned int type_id, unsigned int index) : m_type_id(type_id), m_index(index) {}

  unsigned int m_type_id;
  unsigned int m_index;

  friend struct std::hash<SExtractor::PropertyId>;
};

}

namespace std {

// defines a hash for PropertyId, this is to be able to use PropertyId as a key in a std::unordered_map

template <>
struct hash<SExtractor::PropertyId>
{
  std::size_t operator()(const SExtractor::PropertyId& id) const {
    std::size_t hash = 0;
    boost::hash_combine(hash, id.m_type_id);
    boost::hash_combine(hash, id.m_index);
    return hash;
  }
};

}

#endif
