/**
 * @file SEFramework/Property/PropertyNotFoundException.h
 * @date 05/17/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_PROPERTY_PROPERTYNOTFOUNDEXCEPTION_H
#define _SEFRAMEWORK_PROPERTY_PROPERTYNOTFOUNDEXCEPTION_H

#include "ElementsKernel/Exception.h"

namespace SEFramework {

/**
 * @class PropertyNotFoundException
 * @brief An exception indicating that a Property was not available and could not be computed on demand
 *
 */
class PropertyNotFoundException : public Elements::Exception {
public:
  PropertyNotFoundException() : Elements::Exception("Property not found!") {}
};

} /* namespace SEFramework */


#endif
