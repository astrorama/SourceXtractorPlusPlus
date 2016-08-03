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
 * Properties are accessed through the templated method getProperty<>()/setProperty<>().
 *
 * Actual implementation in sub-classed is done by overriding the virtual getProperty()/setProperty()
 *
 */

class SourceInterface {

public:

  /**
   * @brief Destructor
   */
  virtual ~SourceInterface() = default;

  /// Convenience template method to call getProperty() with a more user-friendly syntax
  template<typename T>
  const T& getProperty(unsigned int index = 0) const {
    return dynamic_cast<const T&>(getProperty(PropertyId::create<T>(index)));
  }

  /// Convenience template method to call setProperty() with a more user-friendly syntax
  template<typename PropertyType, typename ... Args>
  void setIndexedProperty(std::size_t index, Args... args) {
    setProperty(std::unique_ptr<PropertyType>{new PropertyType(std::forward<Args>(args)...)},
                PropertyId::create<PropertyType>(index));
  }
  
  template<typename PropertyType, typename ... Args>
  void setProperty(Args... args) {
    setIndexedProperty<PropertyType>(0, std::forward<Args>(args)...);
  }

protected:
  
  /// Returns a reference to the requested property. The property may be computed if needed
  /// Throws a PropertyNotFoundException if the property cannot be provided.
  virtual const Property& getProperty(const PropertyId& property_id) const = 0;
  virtual void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) = 0;
  
}; /* End of SourceInterface class */

} /* namespace SExtractor */


#endif
