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

class AperturePhotometry : public Property {
public:

  /**
   * @brief Destructor
   */
  virtual ~AperturePhotometry() = default;

  AperturePhotometry(SeFloat flux, SeFloat mag) : m_flux(flux), m_mag(mag) {}

  SeFloat getFlux() const {
    return m_flux;
  }

  SeFloat getMag() const {
    return m_mag;
  }

private:
  SeFloat m_flux;
  SeFloat m_mag;

};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRY_H_ */
