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

#include <algorithm>
#include <functional>

#include <NdArray/NdArray.h>
#include <SEFramework/Image/Image.h>
#include "SEImplementation/Plugin/FluxRadius/FluxRadius.h"
#include "SEImplementation/Plugin/FluxRadius/FluxRadiusTask.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurve.h"

using namespace Euclid::NdArray;

namespace SourceXtractor {

FluxRadiusTask::FluxRadiusTask(const std::vector<unsigned>& instances, const std::vector<SeFloat>& flux_frac)
  : m_instances{instances}, m_flux_fraction{flux_frac} {
}

void FluxRadiusTask::computeProperties(SourceInterface& source) const {
  NdArray<SeFloat> radii{m_instances.size(), m_flux_fraction.size()};

  for (size_t i = 0; i < m_instances.size(); ++i) {
    auto& growth_curve_prop = source.getProperty<GrowthCurve>(m_instances[i]);
    auto& growth_curve = growth_curve_prop.getCurve();
    auto step_size = growth_curve_prop.getStepSize();

    std::vector<double> steps(growth_curve.size());
    for (size_t s = 0; s < steps.size(); ++s) {
      steps[s] = (s + 1) * step_size;
    }

    for (size_t j = 0; j < m_flux_fraction.size(); ++j) {
      auto target_flux = std::max(0., growth_curve.back() * m_flux_fraction[j]);

      // We can not use Alexandria's interpolation because the accumulated flux is not
      // strictly increasing, so we search for the first bin where the accumulated flux is
      // >= the target flux, and interpolate with the previous one
      auto next = std::find_if(std::begin(growth_curve), std::end(growth_curve),
                               std::bind(std::greater_equal<double>(), std::placeholders::_1, target_flux));
      if (next == std::end(growth_curve)) {
        --next;
      }
      size_t next_i = std::distance(std::begin(growth_curve), next);

      SeFloat y0, y1;
      DetectionImage::PixelType x0, x1;

      x1 = *next;
      y1 = steps[next_i];
      if (next_i > 0) {
        x0 = *(next - 1);
        y0 = steps[next_i - 1];
      }
      else {
        x0 = 0;
        y0 = 0;
      }

      SeFloat slope = (y1 - y0) / (x1 - x0);
      SeFloat target_radius = y0 + (target_flux - x0) * slope;
      radii.at(i, j) = std::min(target_radius, static_cast<SeFloat>(steps.back()));
    }
  }
  source.setProperty<FluxRadius>(std::move(radii));
}

} // end of namespace SourceXtractor
