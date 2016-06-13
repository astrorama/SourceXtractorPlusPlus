/**
 * @file src/lib/Property/ObjectWithProperties.cpp
 * @date 05/03/16
 * @author mschefer
 */

#include "SEFramework/Property/PropertyHolder.h"

#include "SEFramework/Property/PropertyNotFoundException.h"

namespace SExtractor {

const Property& PropertyHolder::getProperty(const PropertyId& property_id) const {
  auto iter = m_properties.find(property_id);
  if (iter != m_properties.end()) {
    // Returns the property if it is found
    return *iter->second;
  } else {
    // If we don't have that property throws an exception
    throw PropertyNotFoundException();
  }
}

void PropertyHolder::setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) {
  m_properties[property_id] = std::move(property);
}

bool PropertyHolder::isPropertySet(const PropertyId& property_id) const {
  return m_properties.find(property_id) != m_properties.end();
}

} // SEFramework namespace
