/**
 * @file SEFramework/Source/ModifiableSource.h
 * @date 05/06/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_SOURCE_MODIFIABLESOURCE_H
#define _SEFRAMEWORK_SOURCE_MODIFIABLESOURCE_H

#include "SourceInterface.h"

namespace SEFramework {

/**
 * @class ModifiableSource
 * @brief Extends the SourceInterface by adding the setProperty() method allowing the setting of properties
 *
 * @details SetProperty is a templated method. Sub-classes must override setPropertyImpl() to implement.
 *
 */
class ModifiableSource : public SourceInterface {

public:

  /**
   * @brief Destructor
   */
  virtual ~ModifiableSource() = default;

  template<class T, typename... Ts>
  void setProperty(std::unique_ptr<T> property, Ts&&... params) {
    setPropertyImpl(std::move(property), PropertyId(typeid(T), std::forward<Ts>(params)...) );
  }
protected:
  virtual void setPropertyImpl(std::unique_ptr<Property> property, PropertyId property_id) = 0;

}; /* End of ModifiableSource class */

} /* namespace SEFramework */


#endif
