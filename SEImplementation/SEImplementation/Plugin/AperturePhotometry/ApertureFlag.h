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
 * ApertureFlag.h
 *
 *  Created on: Oct 09, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREFLAG_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREFLAG_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"
#include "SEFramework/Source/SourceFlags.h"
#include <map>

namespace SourceXtractor {

/**
 * @class ApertureFlag
 * @brief Aperture photometry flag
 */
class ApertureFlag : public Property {
public:

  virtual ~ApertureFlag() = default;

  explicit ApertureFlag(const std::map<float, Flags>& flags): m_flags{flags} {}

  const std::map<float, Flags>& getFlags() const {
    return m_flags;
  }

private:
  std::map<float, Flags> m_flags;
};

} /* namespace SourceXtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREFLAG_H_ */
