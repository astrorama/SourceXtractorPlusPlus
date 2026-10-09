/** Copyright © 2019-2026 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
#ifndef _SEIMPLEMENTATION_PLUGIN_SPREADMODEL_SPREADMODELTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_SPREADMODEL_SPREADMODELTASK_H_

#include <cstddef>
#include <map>
#include <string>
#include <vector>

#include "SEFramework/Task/SourceTask.h"

namespace SourceXtractor {

/** Fit one frame's point source, render point/exponential templates, and measure spread.
 * Only m_instance is fitted; the full frame list determines a common rendering scale.
 * Invalid coverage, insufficient pixels, or explicit solver failures yield NaN results.
 */
class SpreadModelTask : public SourceTask {
public:
  explicit SpreadModelTask(unsigned int instance,
                          const std::vector<int>& measurement_frames,
                          unsigned int max_iterations,
                          const std::string& least_squares_engine,
                          double scale_factor,
                          std::size_t max_fit_size,
                          const std::map<int, bool>& should_renormalize)
    : m_instance(instance),
      m_measurement_frames(measurement_frames),
      m_max_iterations(max_iterations),
      m_least_squares_engine(least_squares_engine),
      m_scale_factor(scale_factor),
      m_max_fit_area(static_cast<double>(max_fit_size) * max_fit_size),
      m_should_renormalize(should_renormalize) {}

  virtual ~SpreadModelTask() = default;

  void computeProperties(SourceInterface& source) const override;

private:
  unsigned int m_instance;
  std::vector<int> m_measurement_frames;
  unsigned int m_max_iterations;
  std::string m_least_squares_engine;
  double m_scale_factor;
  // Maximum rendering area in PSF pixels; the configuration specifies a side length.
  double m_max_fit_area;
  std::map<int, bool> m_should_renormalize;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_SPREADMODEL_SPREADMODELTASK_H_ */
