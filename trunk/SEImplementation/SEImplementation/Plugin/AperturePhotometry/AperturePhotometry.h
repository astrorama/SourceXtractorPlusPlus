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

  AperturePhotometry(SeFloat flux) : m_flux(flux) {}

  SeFloat getFlux() const {
    return m_flux;
  }

private:
  SeFloat m_flux;

};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRY_H_ */
