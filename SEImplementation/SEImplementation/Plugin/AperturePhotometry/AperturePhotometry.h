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
 * AperturePhotometry.h
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRY_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRY_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"
#include "SEFramework/Source/SourceFlags.h"
#include <vector>

namespace SExtractor {

/**
 * @class AperturePhotometry
 * @brief Aperture photometry fluxes and magnitudes
 */
class AperturePhotometry : public Property {
public:

  /**
   * @brief Destructor
   */
  virtual ~AperturePhotometry() = default;

  AperturePhotometry(const std::vector<SeFloat> &flux, const std::vector<SeFloat> &flux_error,
                     const std::vector<SeFloat> &mag, const std::vector<SeFloat> &mag_error,
                     const std::vector<Flags> &flag)
    : m_flux(flux), m_flux_error(flux_error), m_mag(mag), m_mag_error(mag_error), m_flag(flag) {}

  const std::vector<SeFloat> &getFluxes() const {
    return m_flux;
  }

  const std::vector<SeFloat> &getFluxErrors() const {
    return m_flux_error;
  }

  const std::vector<SeFloat> &getMags() const {
    return m_mag;
  }

  const std::vector<SeFloat> &getMagErrors() const {
    return m_mag_error;
  }

  const std::vector<Flags> &getFlags() const {
    return m_flag;
  }

private:
  std::vector<SeFloat> m_flux;
  std::vector<SeFloat> m_flux_error;
  std::vector<SeFloat> m_mag;
  std::vector<SeFloat> m_mag_error;
  std::vector<Flags> m_flag;
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRY_H_ */
