/**
 * @file DataModel/SourceInterface.h
 * @date 05/04/16
 * @author mschefer
 */

#ifndef _DATAMODEL_SOURCEINTERFACE_H
#define _DATAMODEL_SOURCEINTERFACE_H

#include <memory>

#include "DataModel/PropertyId.h"
#include "DataModel/PropertyNotFoundException.h"

#include "Property.h"

namespace DataModel {

class Property;

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
  template<class T, typename... Ts>
  T& getProperty(Ts&&... params) const {
    return dynamic_cast<T&>(getPropertyImpl(PropertyId {typeid(T), std::forward<Ts>(params)...} ));
  }

protected:
  /// Implementation used by the template method getProperty()
  virtual Property& getPropertyImpl(const PropertyId property_id) const = 0;

private:
}; /* End of SourceInterface class */

} /* namespace DataModel */


#endif
