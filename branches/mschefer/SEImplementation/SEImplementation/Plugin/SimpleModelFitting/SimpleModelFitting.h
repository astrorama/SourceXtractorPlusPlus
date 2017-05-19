/*
 * SimpleModelFitting.h
 *
 *  Created on: May 2, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTING_H_
#define _SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTING_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

class SimpleModelFitting : public Property {
public:

  virtual ~SimpleModelFitting() = default;

  SimpleModelFitting(SeFloat flux, SeFloat peak, unsigned int iterations) :
    m_flux(flux),
    m_peak(peak),
    m_iterations(iterations) {}

  SeFloat getFlux() const {
    return m_flux;
  }

  SeFloat getPeak() const {
    return m_peak;
  }

  SeFloat getIterations() const {
    return m_iterations;
  }


private:
  SeFloat m_flux;
  SeFloat m_peak;
  unsigned int m_iterations;
};

}




#endif /* _SEIMPLEMENTATION_PLUGIN_SIMPLEMODELFITTING_SIMPLEMODELFITTING_H_ */
