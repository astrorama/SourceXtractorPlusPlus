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
 * AutoPhotometryFlag.h
 *
 *  Created on: Oct 10, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYFLAG_H_
#define _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYFLAG_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"
#include "SEFramework/Source/SourceFlags.h"
#include <vector>

namespace SourceXtractor {

class AutoPhotometryFlag: public Property {
public:
  virtual ~AutoPhotometryFlag() = default;

  explicit AutoPhotometryFlag(const Flags flags): m_flags{flags} {}

  Flags getFlags() const {
    return m_flags;
  }

private:
  Flags m_flags;
};

} /* namespace SourceXtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYFLAG_H_ */
