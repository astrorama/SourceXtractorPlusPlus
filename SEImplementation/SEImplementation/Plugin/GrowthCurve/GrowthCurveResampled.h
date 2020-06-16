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

#ifndef _SEIMPLEMENTATION_PLUGIN_GROWTHCURVE_GROWTHCURVERESAMPLED_H_
#define _SEIMPLEMENTATION_PLUGIN_GROWTHCURVE_GROWTHCURVERESAMPLED_H_

#include <NdArray/NdArray.h>
#include "SEFramework/Property/Property.h"
#include "SEFramework/Image/Image.h"

namespace SourceXtractor {

/**
 * Stores a custom sampling of the growth curve, which is stored under
 * the property GrowthCurve
 */
class GrowthCurveResampled : public Property {
public:
  virtual ~GrowthCurveResampled() = default;

  GrowthCurveResampled(Euclid::NdArray::NdArray<DetectionImage::PixelType>&& samples, std::vector<double>&& step_size)
    : m_samples{std::move(samples)}, m_step_size{std::move(step_size)} {}

  const Euclid::NdArray::NdArray<DetectionImage::PixelType> & getSamples() const {
    return m_samples;
  }

  std::vector<double> getStepSize() const {
    return m_step_size;
  }

private:
  Euclid::NdArray::NdArray<DetectionImage::PixelType> m_samples;
  std::vector<double> m_step_size;
};

} // end of namespace SourceXtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_GROWTHCURVE_GROWTHCURVERESAMPLED_H_ */
