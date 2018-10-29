/*
 * AperturePhotometryTask.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#include <iostream>
#include <SEFramework/Aperture/FluxMeasurement.h>

#include "SEFramework/Aperture/TransformedAperture.h"
#include "SEFramework/Source/SourceFlags.h"
#include "SEImplementation/Plugin/SaturateFlag/SaturateFlag.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Plugin/SourceIDs/SourceID.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlag.h"
#include "SEFramework/Aperture/CircularAperture.h"


namespace SExtractor {

namespace {
const SeFloat CROWD_THRESHOLD_APER = 0.1;
const SeFloat BADAREA_THRESHOLD_APER = 0.1;
}

//////////////////////////////////////////////////////////////////////////////////////////

void AperturePhotometryTask::computeProperties(SourceInterface &source) const {
  auto measurement_frame = source.getProperty<MeasurementFrame>(m_instance).getFrame();
  auto measurement_image = measurement_frame->getSubtractedImage();
  auto variance_map = measurement_frame->getVarianceMap();
  auto variance_threshold = measurement_frame->getVarianceThreshold();
  auto pixel_centroid = source.getProperty<MeasurementFramePixelCentroid>(m_instance);

  // get the object center
  const auto &centroid_x = source.getProperty<MeasurementFramePixelCentroid>(m_instance).getCentroidX();
  const auto &centroid_y = source.getProperty<MeasurementFramePixelCentroid>(m_instance).getCentroidY();

  // m_apertures is the aperture on the detection frame, so we have to wrap it
  // to transform it to the measurement frame
  auto jacobian = source.getProperty<JacobianSource>(m_instance);

  std::vector<SeFloat> fluxes, fluxes_error;
  std::vector<SeFloat> mags, mags_error;
  std::vector<Flags> flags;

  for (auto aperture_size : m_apertures) {
    auto aperture = std::make_shared<TransformedAperture>(std::make_shared<CircularAperture>(aperture_size),
                                                          jacobian.asTuple());

    auto measurement = measureFlux(aperture, centroid_x, centroid_y, measurement_image, variance_map,
                                   variance_threshold, m_use_symmetry);
    // compute the derived quantities
    auto flux_error = sqrt(measurement.m_variance);
    auto mag = measurement.m_flux > 0.0 ? -2.5 * log10(measurement.m_flux) + m_magnitude_zero_point : SeFloat(99.0);
    auto mag_error = 1.0857 * flux_error / measurement.m_flux;

    fluxes.push_back(measurement.m_flux);
    fluxes_error.push_back(flux_error);
    mags.push_back(mag);
    mags_error.push_back(mag_error);
    flags.push_back(measurement.m_flags);
  }

  // Merge flags with those set on the detection frame and from the saturate plugin
  auto aperture_flags = source.getProperty<ApertureFlag>().getFlags();
  auto sat_flag = Flags::SATURATED * source.getProperty<SaturateFlag>().getSaturateFlag();

  for (size_t i = 0; i < aperture_flags.size(); ++i) {
    flags[i] |= aperture_flags[i] | sat_flag;
  }

  // set the source properties
  source.setIndexedProperty<AperturePhotometry>(m_instance, fluxes, fluxes_error, mags, mags_error, flags);

  // Draw the last aperture
  auto aperture = std::make_shared<TransformedAperture>(std::make_shared<CircularAperture>(m_apertures[0]),
                                                        jacobian.asTuple());
  auto coord_system = measurement_frame->getCoordinateSystem();
  auto aperture_check_img = CheckImages::getInstance().getApertureImage(m_instance, measurement_image->getWidth(),
                                                                        measurement_image->getHeight(), coord_system);
  if (aperture_check_img) {
    auto src_id = source.getProperty<SourceID>().getId();
    fillAperture(aperture, centroid_x, centroid_y, aperture_check_img, static_cast<unsigned>(src_id));
  }
}

}

