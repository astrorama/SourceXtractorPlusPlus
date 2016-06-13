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
 * @brief A class providing a simple implementation of an object with properties
 *
 * @details This class is used to provide a common implementation for objects that have properties
 *
 */

class PropertyHolder {

public:

  /**
   * @brief Destructor
   */
  virtual ~PropertyHolder() = default;

  /// Returns a reference to a Property if it is set, if not throws a PropertyNotFoundException
  virtual const Property& getProperty(const PropertyId& property_id) const;

  /// Set a property, overwriting it if necessary
  virtual void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id);

  /// Returns true if the property is set
  virtual bool isPropertySet(const PropertyId& property_id) const;

private:

  std::unordered_map<PropertyId, std::unique_ptr<Property>> m_properties;

}; /* End of ObjectWithProperties class */

} /* namespace SEFramework */


#endif
