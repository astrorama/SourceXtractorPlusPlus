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
#ifndef _SEIMPLEMENTATION_PLUGIN_SPREADMODELALT_JOINTSPREADMODELALT_H_
#define _SEIMPLEMENTATION_PLUGIN_SPREADMODELALT_JOINTSPREADMODELALT_H_

#include <limits>
#include <vector>
#include "SEFramework/Property/Property.h"

namespace SourceXtractor {

/** Joint single-source point/exponential comparison on a fixed set of frames and pixels.
 * Radius is the intrinsic circular exponential half-light radius in reference-image pixels.
 * Delta chi-square is descriptive, not a calibrated detection significance. PSFs and
 * pixel variances are treated as fixed. Boundary solutions have no symmetric radius error.
 */
class JointSpreadModelAlt : public Property {
public:
  enum Flag {
    NONE = 0, ERROR = 1, MAX_ITER = 2, BOUNDARY = 4, OUTSIDE = 8, INSUFFICIENT_DATA = 16
  };

  struct Result {
    double radius = std::numeric_limits<double>::quiet_NaN();
    double radius_error = std::numeric_limits<double>::quiet_NaN();
    double delta_chi_squared = std::numeric_limits<double>::quiet_NaN();
    double point_chi_squared = std::numeric_limits<double>::quiet_NaN();
    double extended_chi_squared = std::numeric_limits<double>::quiet_NaN();
    double point_prior_penalty = std::numeric_limits<double>::quiet_NaN();
    double extended_prior_penalty = std::numeric_limits<double>::quiet_NaN();
    double delta_objective = std::numeric_limits<double>::quiet_NaN();
    double point_offset_rms = std::numeric_limits<double>::quiet_NaN();
    double extended_offset_rms = std::numeric_limits<double>::quiet_NaN();
    // Arrays follow configured frame order, including NaN entries for excluded frames.
    std::vector<int> frame_ids;
    std::vector<double> point_offset_x, point_offset_y;
    std::vector<double> extended_offset_x, extended_offset_y;
    unsigned int n_frames = 0;
    unsigned int n_pixels = 0;
    int flags = NONE;
  };

  explicit JointSpreadModelAlt(const Result& result) : m_result(result) {}
  const Result& getResult() const { return m_result; }

private:
  Result m_result;
};

} // namespace SourceXtractor
#endif
