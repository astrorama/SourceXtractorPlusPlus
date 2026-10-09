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
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

#include "SEImplementation/Plugin/SpreadModel/CombinedSpreadModel.h"
#include "SEImplementation/Plugin/SpreadModel/CombinedSpreadModelTask.h"
#include "SEImplementation/Plugin/SpreadModel/SpreadModel.h"
#include "SEUtils/IsNan.h"

namespace SourceXtractor {

void CombinedSpreadModelTask::computeProperties(SourceInterface& source) const {
  std::vector<std::pair<double, double>> measurements;
  double min_error = std::numeric_limits<double>::max();
  // Request actual frame IDs through the property system, which computes and
  // caches missing measurements. Combining never replaces the indexed properties.
  for (auto frame : m_frames) {
    const auto& measurement = source.getProperty<SpreadModel>(frame);
    const double value = measurement.getSpreadModel();
    const double error = measurement.getSpreadModelError();
    if (fastmath_isnan(value) || fastmath_isinf(value) || fastmath_isnan(error) || fastmath_isinf(error) ||
        error <= 0) {
      continue;
    }
    measurements.emplace_back(value, error);
    min_error = std::min(min_error, error);
  }

  const double nan = std::numeric_limits<double>::quiet_NaN();
  if (measurements.empty()) {
    source.setProperty<CombinedSpreadModel>(nan, nan, 0, nan);
    return;
  }
  if (measurements.size() == 1) {
    // Preserve the lone measurement exactly; frame agreement is undefined for N < 2.
    source.setProperty<CombinedSpreadModel>(measurements.front().first, measurements.front().second, 1, nan);
    return;
  }

  // w = (min_error / sigma)^2 is proportional to inverse variance but stays <= 1.
  // At least one weight equals 1, keeping the denominator nonzero without computing
  // potentially overflowing 1/sigma^2. Restore the error's scale after summation.
  double weighted_sum = 0;
  double weight_sum = 0;
  for (const auto& measurement : measurements) {
    const double ratio = min_error / measurement.second;
    const double weight = ratio * ratio;
    weighted_sum += weight * measurement.first;
    weight_sum += weight;
  }
  const double mean = weighted_sum / weight_sum;
  const double error = min_error / std::sqrt(weight_sum);
  // One fitted mean leaves N-1 degrees of freedom. This is a diagnostic only:
  // no clipping, inter-frame covariance correction, or error inflation is applied.
  double chi_squared = 0;
  for (const auto& measurement : measurements) {
    const double residual = (measurement.first - mean) / measurement.second;
    chi_squared += residual * residual;
  }
  source.setProperty<CombinedSpreadModel>(mean, error, measurements.size(),
      chi_squared / (measurements.size() - 1));
}

} // namespace SourceXtractor
