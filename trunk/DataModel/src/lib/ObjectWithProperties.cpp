/**
 * @file src/lib/ObjectWithProperties.cpp
 * @date 05/03/16
 * @author mschefer
 */

#include "DataModel/ObjectWithProperties.h"
#include "DataModel/PropertyNotFoundException.h"

namespace DataModel {

Property& ObjectWithProperties::getPropertyImpl(const PropertyId property_id) const {
  auto iter = m_properties.find(property_id);
  if (iter != m_properties.end()) {
    // Returns the property if it is found
    return *iter->second;
  } else {
    // If we don't have that property throws an exception
    throw PropertyNotFoundException();
  }
}

void ObjectWithProperties::setPropertyImpl(std::unique_ptr<Property> property, PropertyId property_id) {
  m_properties[property_id] = std::move(property);
}

bool ObjectWithProperties::isPropertySet(const PropertyId property_id) const {
  return m_properties.find(property_id) != m_properties.end();
}

} // DataModel namespace
