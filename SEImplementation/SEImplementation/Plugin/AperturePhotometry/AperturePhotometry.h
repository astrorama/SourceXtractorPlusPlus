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

namespace SExtractor {

/**
 * @class AperturePhotometry
 * @brief Aperture photometry flux and magnitude
 */
class AperturePhotometry : public Property {
public:

  /**
   * @brief Destructor
   */
  virtual ~AperturePhotometry() = default;

  AperturePhotometry(SeFloat flux, SeFloat flux_error, SeFloat mag, SeFloat mag_error, long int flag)
    : m_flux(flux), m_flux_error(flux_error), m_mag(mag), m_mag_error(mag_error), m_flag(flag) {}

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

  long int getFlag() const {
    return m_flag;
  }

private:
  SeFloat  m_flux;
  SeFloat  m_flux_error;
  SeFloat  m_mag;
  SeFloat  m_mag_error;
  long int m_flag;
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRY_H_ */
