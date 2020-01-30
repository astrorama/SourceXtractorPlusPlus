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
 * IsophotalTask.cpp
 *
 *  Created on: Sep 28, 2016
 *      Author: mschefer
 */

#include "SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h"

#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFluxTask.h"

namespace SourceXtractor {

void IsophotalFluxTask::computeProperties(SourceInterface& source) const {
  const auto& detection_frame = source.getProperty<DetectionFrame>();
  const auto& pixel_values = source.getProperty<DetectionFramePixelValues>().getValues();
  const auto& pixel_variances = source.getProperty<DetectionFramePixelValues>().getVariances();

  SeFloat total_flux = 0.0;
  SeFloat total_variance = 0.0;

  auto variance_iter = pixel_variances.begin();
  for (auto value : pixel_values) {
    auto variance = *variance_iter++;

    total_flux += value;
    total_variance += variance;
  }

  // Add variance from gain
  SeFloat gain = detection_frame.getFrame()->getGain();
  if (gain >0.0)
    total_variance += total_flux / gain;

  auto flux_error = sqrt(total_variance);

  auto mag = total_flux > 0.0 ? -2.5*log10(total_flux) + m_magnitude_zero_point : std::numeric_limits<SeFloat>::quiet_NaN();
  auto mag_error = 1.0857 * flux_error / total_flux;

  source.setProperty<IsophotalFlux>(total_flux, flux_error, mag, mag_error);
}

}


