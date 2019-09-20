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
 * @file SEImplementation/IsophotalFlux.h
 * @date 09/28/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_ISOPHOTALFLUX_H
#define _SEIMPLEMENTATION_PROPERTY_ISOPHOTALFLUX_H

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

/**
 * @class IsophotalFlux
 * @brief Computes the isophotal flux and magnitude
 * @details Isophotal flux is the total flux from the detection pixels
 */
class IsophotalFlux : public Property {
public:

  /**
   * @brief Destructor
   */
  virtual ~IsophotalFlux() = default;

  IsophotalFlux(SeFloat flux, SeFloat flux_error, SeFloat mag, SeFloat mag_error)
    : m_flux(flux), m_flux_error(flux_error), m_mag(mag), m_mag_error(mag_error) {}

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

private:
  SeFloat m_flux;
  SeFloat m_flux_error;
  SeFloat m_mag;
  SeFloat m_mag_error;

}; /* End of IsophotalFlux class */

} /* namespace SExtractor */


#endif
