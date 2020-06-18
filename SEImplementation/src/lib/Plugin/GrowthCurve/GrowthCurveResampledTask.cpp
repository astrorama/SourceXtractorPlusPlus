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

#include <MathUtils/interpolation/interpolation.h>

#include "SEImplementation/Plugin/GrowthCurve/GrowthCurve.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurveResampled.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurveResampledTask.h"

using namespace Euclid::MathUtils;
using namespace Euclid::NdArray;

namespace SourceXtractor {

GrowthCurveResampledTask::GrowthCurveResampledTask(const std::vector<unsigned>& instances, size_t nsamples)
  : m_instances{instances}, m_nsamples{nsamples} {}

void GrowthCurveResampledTask::computeProperties(SourceInterface& source) const {
  NdArray<DetectionImage::PixelType> data{m_instances.size(), m_nsamples};
  std::vector<double> step_sizes(m_instances.size());

  for (size_t i = 0; i < m_instances.size(); ++i) {
    auto& growth_curve_prop = source.getProperty<GrowthCurve>(m_instances[i]);
    auto& growth_curve = growth_curve_prop.getCurve();
    auto step_size = growth_curve_prop.getStepSize();
    auto new_step_size = growth_curve_prop.getMax() / m_nsamples;
    step_sizes[i] = new_step_size;

    std::vector<double> steps(growth_curve.size());
    for (size_t s = 0; s < steps.size(); ++s) {
      steps[s] = (s + 1) * step_size;
    }

    auto interpolated = interpolate(steps, growth_curve, InterpolationType::LINEAR, true);
    for (size_t s = 0; s < m_nsamples; ++s) {
      data.at(i, s) = (*interpolated)((s + 1) * new_step_size);
    }
  }
  source.setProperty<GrowthCurveResampled>(std::move(data), std::move(step_sizes));
}

} // end of namespace SourceXtractor
