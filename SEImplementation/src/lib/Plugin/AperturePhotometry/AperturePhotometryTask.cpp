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
 * AperturePhotometryTask.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#include "SEFramework/Aperture/CircularAperture.h"
#include "SEFramework/Aperture/FluxMeasurement.h"
#include "SEFramework/Aperture/TransformedAperture.h"
#include "SEFramework/Source/SourceFlags.h"

#include "SEImplementation/CheckImages/CheckImages.h"

#include "SEImplementation/Plugin/BlendedFlag/BlendedFlag.h"
#include "SEImplementation/Plugin/SaturateFlag/SaturateFlag.h"
#include <SEImplementation/Plugin/MeasurementFrameInfo/MeasurementFrameInfo.h>
#include <SEImplementation/Plugin/MeasurementFrameImages/MeasurementFrameImages.h>
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "SEImplementation/Plugin/SourceIDs/SourceID.h"

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlag.h"

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"

namespace SourceXtractor {

//////////////////////////////////////////////////////////////////////////////////////////

void AperturePhotometryTask::computeProperties(SourceInterface &source) const {
  const auto& measurement_frame_info = source.getProperty<MeasurementFrameInfo>(m_instance);
  const auto& measurement_frame_images = source.getProperty<MeasurementFrameImages>(m_instance);

  auto variance_threshold = measurement_frame_info.getVarianceThreshold();
  auto gain = measurement_frame_info.getGain();

  const auto measurement_image = measurement_frame_images.getLockedImage(LayerSubtractedImage);
  const auto variance_map = measurement_frame_images.getLockedImage(LayerVarianceMap);

  auto pixel_centroid = source.getProperty<MeasurementFramePixelCentroid>(m_instance);

  // get the object center
  const auto& centroid_x = source.getProperty<MeasurementFramePixelCentroid>(m_instance).getCentroidX();
  const auto& centroid_y = source.getProperty<MeasurementFramePixelCentroid>(m_instance).getCentroidY();

  // m_apertures is the aperture on the detection frame, so we have to wrap it
  // to transform it to the measurement frame
  auto jacobian = source.getProperty<JacobianSource>(m_instance);

  std::vector<SeFloat> fluxes, fluxes_error;
  std::vector<SeFloat> mags, mags_error;
  std::vector<Flags> flags;

  for (auto aperture_diameter : m_apertures) {
    auto aperture = std::make_shared<TransformedAperture>(
      std::make_shared<CircularAperture>(aperture_diameter / 2.),
      jacobian.asTuple()
    );

    auto measurement = measureFlux(aperture, centroid_x, centroid_y, measurement_image, variance_map,
                                   variance_threshold, m_use_symmetry);
    // compute the derived quantities
    if (gain > 0) {
      measurement.m_variance += measurement.m_flux / gain;
    }
    auto flux_error = sqrt(measurement.m_variance);
    auto mag = measurement.m_flux > 0.0 ? -2.5 * log10(measurement.m_flux) + m_magnitude_zero_point : std::numeric_limits<SeFloat>::quiet_NaN();
    auto mag_error = 1.0857 * flux_error / measurement.m_flux;

    fluxes.push_back(measurement.m_flux);
    fluxes_error.push_back(flux_error);
    mags.push_back(mag);
    mags_error.push_back(mag_error);
    flags.push_back(measurement.m_flags);
  }

  // Merge flags with those set on the detection frame and from the saturate and blended plugins
  Flags additional_flags(Flags::NONE);
  additional_flags |= Flags::SATURATED * source.getProperty<SaturateFlag>(m_instance).getSaturateFlag();
  additional_flags |= Flags::BLENDED * source.getProperty<BlendedFlag>().getBlendedFlag();

  auto aperture_flags = source.getProperty<ApertureFlag>().getFlags();
  for (size_t i = 0; i < m_apertures.size(); ++i) {
    auto det_flag = aperture_flags.at(m_apertures[i]);
    flags[i] |= additional_flags | det_flag;
  }

  // set the source properties
  source.setIndexedProperty<AperturePhotometry>(m_instance, fluxes, fluxes_error, mags, mags_error, flags);

  // draw the apertures onto the checkimage
  auto aperture_check_img = CheckImages::getInstance().getMeasurementApertureImage(m_instance);
  if (aperture_check_img) {
    auto src_id = source.getProperty<SourceID>().getId();
    for (auto aperture_diameter : m_apertures) {
    	auto aperture = std::make_shared<TransformedAperture>(std::make_shared<CircularAperture>(aperture_diameter / 2.),
    			jacobian.asTuple());
    		drawAperture<int>(aperture, centroid_x, centroid_y, aperture_check_img, static_cast<unsigned>(src_id));
    }

  }
}

}

