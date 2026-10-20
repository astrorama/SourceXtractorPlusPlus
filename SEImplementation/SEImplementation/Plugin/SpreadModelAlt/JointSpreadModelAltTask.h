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
#ifndef _SEIMPLEMENTATION_PLUGIN_SPREADMODELALT_JOINTSPREADMODELALTTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_SPREADMODELALT_JOINTSPREADMODELALTTASK_H_

#include <cstddef>
#include <map>
#include <stdexcept>
#include "SEUtils/IsNan.h"
#include <string>
#include <vector>
#include "SEFramework/Task/SourceTask.h"

namespace SourceXtractor {

/** Fit one position and intrinsic radius across frames, with regularized frame offsets and independent fluxes. */
class JointSpreadModelAltTask : public SourceTask {
public:
  JointSpreadModelAltTask(const std::vector<int>& frames, unsigned int max_iterations,
                         const std::string& engine, double scale_factor, std::size_t max_fit_size,
                         const std::map<int, bool>& renormalize, double position_prior_sigma = 0.2)
      : m_frames(frames), m_max_iterations(max_iterations), m_engine(engine), m_scale_factor(scale_factor),
        m_max_fit_area(double(max_fit_size) * max_fit_size), m_renormalize(renormalize),
        m_position_prior_sigma(position_prior_sigma) {
    if (fastmath_isnan(position_prior_sigma) || fastmath_isinf(position_prior_sigma) || position_prior_sigma < 0 ||
        (position_prior_sigma > 0 && fastmath_isinf(1 / position_prior_sigma))) {
      throw std::invalid_argument("Position prior sigma must be finite and nonnegative with finite inverse");
    }
  }

  void computeProperties(SourceInterface& source) const override;

private:
  std::vector<int> m_frames;
  unsigned int m_max_iterations;
  std::string m_engine;
  double m_scale_factor;
  double m_max_fit_area;
  std::map<int, bool> m_renormalize;
  double m_position_prior_sigma;
};

} // namespace SourceXtractor
#endif
