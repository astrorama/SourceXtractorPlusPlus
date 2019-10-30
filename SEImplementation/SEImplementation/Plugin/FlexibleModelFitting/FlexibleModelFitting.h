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
 * FlexibleModelFitting.h
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTING_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTING_H_

#include <unordered_map>
#include <ElementsKernel/Logging.h>

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"
#include "SEFramework/Source/SourceFlags.h"

namespace SourceXtractor {

extern Elements::Logging model_fitting_logger;

// FlexibleModelFitting property class to contain the results from the model fitting

class FlexibleModelFitting : public Property {
public:
  virtual ~FlexibleModelFitting() = default;

  FlexibleModelFitting(unsigned int iterations, SeFloat chi_squared, Flags flags,
      std::unordered_map<int, double> parameter_values, std::unordered_map<int, double> parameter_sigmas) :
    m_iterations(iterations),
    m_chi_squared(chi_squared),
    m_flags(flags),
    m_parameter_values(parameter_values),
    m_parameter_sigmas(parameter_sigmas) {}

  unsigned int getIterations() const {
    return m_iterations;
  }

  SeFloat getReducedChiSquared() const {
    return m_chi_squared;
  }

  Flags getFlags() const {
    return m_flags;
  }

  SeFloat getParameterValue(int index) const {
    return m_parameter_values.at(index);
  }

  SeFloat getParameterSigma(int index) const {
    return m_parameter_sigmas.at(index);
  }

private:
  unsigned int m_iterations;
  SeFloat m_chi_squared;
  Flags m_flags;
  std::unordered_map<int, double> m_parameter_values;
  std::unordered_map<int, double> m_parameter_sigmas;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTING_H_ */
