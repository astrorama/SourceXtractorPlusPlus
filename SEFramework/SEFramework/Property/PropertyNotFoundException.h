/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/**
 * @file SEFramework/Property/PropertyNotFoundException.h
 * @date 05/17/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_PROPERTY_PROPERTYNOTFOUNDEXCEPTION_H
#define _SEFRAMEWORK_PROPERTY_PROPERTYNOTFOUNDEXCEPTION_H

#include "ElementsKernel/Exception.h"
#include "SEFramework/Property/PropertyId.h"

namespace SourceXtractor {

/**
 * @class PropertyNotFoundException
 * @brief An exception indicating that a Property was not available and could not be computed on demand
 *
 */
class PropertyNotFoundException : public Elements::Exception {
public:
  explicit PropertyNotFoundException(PropertyId property_id) : Elements::Exception(
      std::string("Property ") + property_id.getString() + " not found!") {}
};

} /* namespace SourceXtractor */


#endif
