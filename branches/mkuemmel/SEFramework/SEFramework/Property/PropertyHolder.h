/**
 * @file SEFramework/Property/ObjectWithProperties.h
 * @date 05/03/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_PROPERTY_PROPERTYHOLDER_H
#define _SEFRAMEWORK_PROPERTY_PROPERTYHOLDER_H

#include <memory>
#include <unordered_map>

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {


/**
 * @class PropertyHolder
 * @brief A class providing a simple implementation of a container of properties.
 *
 * @details This class is used to provide a common implementation for objects that have properties
 *
 */

class PropertyHolder {

public:

  /// Destructor
  virtual ~PropertyHolder() = default;

  // removes copy/move constructors and assignment operators
  PropertyHolder(const PropertyHolder&) = delete;
  PropertyHolder& operator=(const PropertyHolder&) = delete;
  PropertyHolder(PropertyHolder&&) = delete;
  PropertyHolder& operator=(PropertyHolder&&) = delete;

  /// Constructors
  PropertyHolder() {}

  /// Returns a reference to a Property if it is set, if not throws a PropertyNotFoundException
  const Property& getProperty(const PropertyId& property_id) const;

  /// Sets a property, overwriting it if necessary
  void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id);

  /// Returns true if the property is set
  bool isPropertySet(const PropertyId& property_id) const;
  
  void clear();

private:

  std::unordered_map<PropertyId, std::unique_ptr<Property>> m_properties;

}; /* End of ObjectWithProperties class */

} /* namespace SExtractor */


#endif
