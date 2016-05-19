/**
 * @file DataModel/ObjectWithProperties.h
 * @date 05/03/16
 * @author mschefer
 */

#ifndef _DATAMODEL_OBJECTWITHPROPERTIES_H
#define _DATAMODEL_OBJECTWITHPROPERTIES_H

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <string>

#include "PropertyId.h"
#include "Property.h"

namespace DataModel {


/**
 * @class ObjectWithProperties
 * @brief A class providing a simple implementation of an object with properties
 *
 * @details This class is used to provide a common implementation of collection properties
 *
 */

class ObjectWithProperties {

public:

  /**
   * @brief Destructor
   */
  virtual ~ObjectWithProperties() = default;

  /// Returns a reference to a Property if it is set, if not throws a PropertyNotFoundException
  virtual Property& getPropertyImpl(const PropertyId property_id) const;

  /// Set a property, overwriting it if necessary
  virtual void setPropertyImpl(std::unique_ptr<Property> property, PropertyId property_id);

  /// Returns true if the property is set
  virtual bool isPropertySet(const PropertyId property_id) const;

private:

  std::unordered_map<PropertyId, std::unique_ptr<Property>> m_properties;

}; /* End of ObjectWithProperties class */

} /* namespace DataModel */


#endif
