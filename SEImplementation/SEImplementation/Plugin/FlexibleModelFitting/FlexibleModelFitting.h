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

  FlexibleModelFitting(unsigned int iterations, unsigned int stop_reason,
                       SeFloat chi_squared, SeFloat duration, Flags flags,
                       std::unordered_map<int, double> parameter_values,
                       std::unordered_map<int, double> parameter_sigmas,
                       std::vector<SeFloat> chi_squared_per_meta,
                       std::vector<int> iterations_per_meta,
                       int meta_iterations,
                       std::vector<SeFloat> fitting_areas_x,
                       std::vector<SeFloat> fitting_areas_y) :
    m_iterations(iterations),
    m_stop_reason(stop_reason),
    m_chi_squared(chi_squared),
    m_duration(duration),
    m_flags(flags),
    m_parameter_values(parameter_values),
    m_parameter_sigmas(parameter_sigmas),
    m_chi_squared_per_meta(chi_squared_per_meta),
    m_iterations_per_meta(iterations_per_meta),
    m_meta_iterations(meta_iterations),
    m_fitting_areas_x(fitting_areas_x),
    m_fitting_areas_y(fitting_areas_y)
{}

  unsigned int getIterations() const {
    return m_iterations;
  }

  unsigned int getStopReason() const {
    return m_stop_reason;
  }

  SeDouble getReducedChiSquared() const {
    return m_chi_squared;
  }

  Flags getFlags() const {
    return m_flags;
  }

  SeDouble getParameterValue(int index) const {
    return m_parameter_values.at(index);
  }

  SeDouble getParameterSigma(int index) const {
    return m_parameter_sigmas.at(index);
  }

  SeFloat getDuration() const {
    return m_duration;
  }

  std::vector<SeFloat> getChiSquaredPerMetaIteration() const {
    return m_chi_squared_per_meta;
  }

  std::vector<int> getIterationsPerMetaIteration() const {
    return m_iterations_per_meta;
  }

  int getMetaIterations() const {
    return m_meta_iterations;
  }

  std::vector<SeFloat> getFittingAreasX() const {
    return m_fitting_areas_x;
  }

  std::vector<SeFloat> getFittingAreasY() const {
    return m_fitting_areas_y;
  }

private:
  unsigned int m_iterations, m_stop_reason;
  SeFloat m_chi_squared, m_duration;
  Flags m_flags;
  std::unordered_map<int, double> m_parameter_values;
  std::unordered_map<int, double> m_parameter_sigmas;

  std::vector<SeFloat> m_chi_squared_per_meta;
  std::vector<int> m_iterations_per_meta;
  int m_meta_iterations;

  std::vector<SeFloat> m_fitting_areas_x;
  std::vector<SeFloat> m_fitting_areas_y;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTING_H_ */
