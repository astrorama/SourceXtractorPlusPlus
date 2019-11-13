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
 *  Created on: Jul 18, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRY_H_
#define _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRY_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"
#include "SEFramework/Source/SourceFlags.h"

namespace SExtractor {

/**
 * @class AutoPhotometry
 * @brief Auto photometry flux and magnitude
 */
class AutoPhotometry : public Property {
public:

  /**
   * @brief Destructor
   */
  virtual ~AutoPhotometry() = default;

  AutoPhotometry(SeFloat flux, SeFloat flux_error, SeFloat mag, SeFloat mag_error, Flags flags)
    : m_flux(flux), m_flux_error(flux_error), m_mag(mag), m_mag_error(mag_error), m_flags(flags) {}

  SeFloat getFlux() const {
    return m_flux;
  }

  SeFloat getFluxError() const {
    return m_flux_error;
  }

  SeFloat getMag() const {
    return m_mag;
  }

  SeFloat getMagError() const {
    return m_mag_error;
  }

  Flags getFlags() const {
    return m_flags;
  }

private:
  SeFloat  m_flux;
  SeFloat  m_flux_error;
  SeFloat  m_mag;
  SeFloat  m_mag_error;
  Flags m_flags;
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRY_H_ */
