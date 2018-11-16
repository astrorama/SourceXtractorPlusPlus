/*
 * FlexibleModelFitting.h
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTING_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTING_H_

#include <unordered_map>

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

// FlexibleModelFitting property class to contain the results from the model fitting

class FlexibleModelFitting : public Property {
public:
  virtual ~FlexibleModelFitting() = default;

  FlexibleModelFitting(unsigned int iterations, SeFloat chi_squared, std::unordered_map<int, double> parameter_values) :
    m_iterations(iterations),
    m_chi_squared(chi_squared),
    m_parameter_values(parameter_values) {}

  unsigned int getIterations() const {
    return m_iterations;
  }

  SeFloat getReducedChiSquared() const {
    return m_chi_squared;
  }

  SeFloat getParameterValue(int index) const {
    return m_parameter_values.at(index);
  }

private:
  unsigned int m_iterations;
  SeFloat m_chi_squared;
  std::unordered_map<int, double> m_parameter_values;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTING_H_ */
