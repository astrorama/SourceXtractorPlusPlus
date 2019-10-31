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
 * AutoPhotometryArray.h
 *
 *  Created on: Nov 23, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYARRAY_H_
#define _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYARRAY_H_

#include <vector>
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometry.h"

namespace SourceXtractor {

/**
 * @class AutoPhotometry
 * @brief Auto photometry flux and magnitude, merging the measurements per frame into a single
 *        property.
 */
class AutoPhotometryArray : public Property {
public:

  /**
   * @brief Destructor
   */
  virtual ~AutoPhotometryArray() = default;

  AutoPhotometryArray(const std::vector<AutoPhotometry> &measurements) {
    for (auto& m : measurements) {
      m_fluxes.push_back(m.getFlux());
      m_flux_errors.push_back(m.getFluxError());
      m_mags.push_back(m.getMag());
      m_mag_errors.push_back(m.getMagError());
      m_flags.push_back(m.getFlags());
    }
  }

  const std::vector<SeFloat> &getFluxes() const {
    return m_fluxes;
  }

  const std::vector<SeFloat> &getFluxErrors() const {
    return m_flux_errors;
  }

  const std::vector<SeFloat> &getMags() const {
    return m_mags;
  }

  const std::vector<SeFloat> &getMagErrors() const {
    return m_mag_errors;
  }

  const std::vector<Flags> &getFlags() const {
    return m_flags;
  }

private:
  std::vector<SeFloat> m_fluxes;
  std::vector<SeFloat> m_flux_errors;
  std::vector<SeFloat> m_mags;
  std::vector<SeFloat> m_mag_errors;
  std::vector<Flags> m_flags;
};

} // end SourceXtractor

#endif // _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYARRAY_H_
