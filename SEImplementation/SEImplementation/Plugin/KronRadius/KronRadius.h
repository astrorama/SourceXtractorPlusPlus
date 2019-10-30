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
 * AutoPhotometry.h
 *
 *  Created on: Sept. 12, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUS_H_
#define _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUS_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SourceXtractor {

/**
 * @class KronRadius
 * @brief Kron radius
 */
class KronRadius : public Property {
public:

  /**
   * @brief Destructor
   */
  virtual ~KronRadius() = default;

  KronRadius(SeFloat kron_radius, long int flag)
    : m_kron_radius(kron_radius), m_flag(flag) {}

  SeFloat getKronRadius() const {
    return m_kron_radius;
  }

  SeFloat getFlag() const {
    return m_flag;
  }

private:
  SeFloat  m_kron_radius;
  long int m_flag;
};

} /* namespace SourceXtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUS_H_ */
