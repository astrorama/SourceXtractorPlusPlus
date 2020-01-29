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
/*
 * SimpleSource.h
 *
 *  Created on: Aug 9, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_SOURCE_SIMPLESOURCE_H_
#define _SEFRAMEWORK_SOURCE_SIMPLESOURCE_H_

#include "SEFramework/Source/SourceWithOnDemandProperties.h"

namespace SourceXtractor {

class SimpleSource: public SourceWithOnDemandProperties {
public:
  SimpleSource(): SourceWithOnDemandProperties(nullptr) {}
};

} /* namespace SourceXtractor */

#endif /* _SEFRAMEWORK_SOURCE_SIMPLESOURCE_H_ */
