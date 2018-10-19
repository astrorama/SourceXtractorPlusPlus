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
                     const std::vector<long> &flag)
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

  const std::vector<long> &getFlags() const {
    return m_flag;
  }

private:
  std::vector<SeFloat> m_flux;
  std::vector<SeFloat> m_flux_error;
  std::vector<SeFloat> m_mag;
  std::vector<SeFloat> m_mag_error;
  std::vector<long> m_flag;
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRY_H_ */
