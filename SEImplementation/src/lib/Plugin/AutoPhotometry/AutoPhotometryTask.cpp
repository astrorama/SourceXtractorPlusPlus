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
 * AutoPhotometryTask.cpp
 *
 *  Created on: Jul 18, 2016
 *      Author: mkuemmel@usm.lmu.de
 */

#include "SEFramework/Aperture/EllipticalAperture.h"
#include "SEFramework/Aperture/TransformedAperture.h"
#include "SEFramework/Source/SourceFlags.h"
#include "SEFramework/Aperture/FluxMeasurement.h"
#include "SEImplementation/Measurement/MultithreadedMeasurement.h"
#include "SEImplementation/Plugin/BlendedFlag/BlendedFlag.h"
#include "SEImplementation/Plugin/SaturateFlag/SaturateFlag.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryFlag.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/KronRadius/KronRadius.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Plugin/SourceIDs/SourceID.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometry.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryTask.h"


namespace SourceXtractor {

//////////////////////////////////////////////////////////////////////////////////////////

void AutoPhotometryTask::computeProperties(SourceInterface &source) const {
  std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

  // get the measurement frame
  const auto& measurement_frame = source.getProperty<MeasurementFrame>(m_instance).getFrame();

  // get the images and image information from the frame
  const auto& measurement_image = measurement_frame->getSubtractedImage();
  const auto& variance_map = measurement_frame->getVarianceMap();
  const auto& variance_threshold = measurement_frame->getVarianceThreshold();
  SeFloat gain = measurement_frame->getGain();

  // get the object center
  const auto& centroid_x = source.getProperty<MeasurementFramePixelCentroid>(m_instance).getCentroidX();
  const auto& centroid_y = source.getProperty<MeasurementFramePixelCentroid>(m_instance).getCentroidY();

  // get the shape parameters
  const auto& cxx = source.getProperty<ShapeParameters>().getEllipseCxx();
  const auto& cyy = source.getProperty<ShapeParameters>().getEllipseCyy();
  const auto& cxy = source.getProperty<ShapeParameters>().getEllipseCxy();

  // get the jacobian
  const auto& jacobian = source.getProperty<JacobianSource>(m_instance);

  // get the kron-radius
  SeFloat kron_radius_auto = m_kron_factor * source.getProperty<KronRadius>().getKronRadius();
  if (kron_radius_auto < m_kron_minrad)
    kron_radius_auto = m_kron_minrad;

  // create the elliptical aperture
  auto ell_aper = std::make_shared<TransformedAperture>(
    std::make_shared<EllipticalAperture>(cxx, cyy, cxy, kron_radius_auto),
    jacobian.asTuple());

  auto measurement = measureFlux(ell_aper, centroid_x, centroid_y, measurement_image, variance_map, variance_threshold,
                                 m_use_symmetry);

  // compute the derived quantities
  auto total_variance = measurement.m_variance;
  if (gain >0.0)
	  total_variance += measurement.m_flux / gain;
  auto flux_error = sqrt(total_variance);
  auto mag = measurement.m_flux > 0.0 ? -2.5 * log10(measurement.m_flux) + m_magnitude_zero_point : std::numeric_limits<SeFloat>::quiet_NaN();
  auto mag_error = 1.0857 * flux_error / measurement.m_flux;

  // Add the flags from the detection image and from the saturated and blended plugins
  measurement.m_flags |= source.getProperty<AutoPhotometryFlag>().getFlags();
  measurement.m_flags |= Flags::SATURATED * source.getProperty<SaturateFlag>(m_instance).getSaturateFlag();
  measurement.m_flags |= Flags::BLENDED * source.getProperty<BlendedFlag>().getBlendedFlag();

  // set the source properties
  source.setIndexedProperty<AutoPhotometry>(m_instance, measurement.m_flux, flux_error, mag, mag_error, measurement.m_flags);

  // Draw the aperture
  auto coord_system = measurement_frame->getCoordinateSystem();
  auto aperture_check_img = CheckImages::getInstance().getAutoApertureImage(measurement_frame);
  if (aperture_check_img) {
    auto src_id = source.getProperty<SourceID>().getId();

    fillAperture(ell_aper, centroid_x, centroid_y, aperture_check_img, static_cast<unsigned>(src_id));
  }
}

}
