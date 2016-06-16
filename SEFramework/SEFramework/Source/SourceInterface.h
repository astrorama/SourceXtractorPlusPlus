/**
 * @file SEFramework/Source/SourceInterface.h
 * @date 05/04/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_SOURCE_SOURCEINTERFACE_H
#define _SEFRAMEWORK_SOURCE_SOURCEINTERFACE_H

#include <memory>

#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Property/PropertyNotFoundException.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

/**
 * @class SourceInterface
 * @brief The SourceInterface is an abstract "source" that has properties attached to it.
 *
 * @details
 * Properties are accessed through the templated method getProperty<>().
 *
 * Actual implementation in sub-classed is done by overriding getPropertyImpl()
 *
 */

class SourceInterface {

public:

  /**
   * @brief Destructor
   */
  virtual ~SourceInterface() = default;

  /// Returns a reference to the requested property. The property may be computed if needed
  /// Throws a PropertyNotFoundException if the property cannot be provided.

  virtual const Property& getProperty(const PropertyId& property_id) const = 0;
  virtual void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) = 0;

  /// Convenience template method to call getProperty() with a more user-friendly syntax
  template<typename T>
  const T& getProperty(unsigned int index = 0) const {
    return dynamic_cast<const T&>(getProperty(PropertyId::create<T>(index)));
  }

  /// Convenience template method to call setProperty() with a more user-friendly syntax
  template<typename T>
  void setProperty(std::unique_ptr<T> property, unsigned int index = 0) {
    setProperty(std::move(property), PropertyId::create<T>(index));
  }


private:
}; /* End of SourceInterface class */

} /* namespace SEFramework */


#endif
