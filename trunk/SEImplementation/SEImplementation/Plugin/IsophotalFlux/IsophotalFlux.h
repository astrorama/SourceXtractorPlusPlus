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
 * @brief
 */
class IsophotalFlux : public Property {
public:

  /**
   * @brief Destructor
   */
  virtual ~IsophotalFlux() = default;

  IsophotalFlux(SeFloat flux, SeFloat mag) : m_flux(flux), m_mag(mag) {}

  SeFloat getFlux() const {
    return m_flux;
  }

  SeFloat getMag() const {
    return m_mag;
  }

private:
  SeFloat m_flux;
  SeFloat m_mag;

}; /* End of IsophotalFlux class */

} /* namespace SExtractor */


#endif
