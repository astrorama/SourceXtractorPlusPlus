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

  AutoPhotometry(SeFloat flux, SeFloat flux_error, SeFloat mag, SeFloat mag_error, Flags flag)
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

  Flags getFlag() const {
    return m_flag;
  }

private:
  SeFloat  m_flux;
  SeFloat  m_flux_error;
  SeFloat  m_mag;
  SeFloat  m_mag_error;
  Flags m_flag;
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRY_H_ */