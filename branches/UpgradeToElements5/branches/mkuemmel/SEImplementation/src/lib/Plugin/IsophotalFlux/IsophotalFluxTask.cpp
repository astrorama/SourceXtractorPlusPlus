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

namespace SExtractor {

void IsophotalFluxTask::computeProperties(SourceInterface& source) const {
  const auto& pixel_values = source.getProperty<DetectionFramePixelValues>().getValues();
  const auto& pixel_coordinates = source.getProperty<PixelCoordinateList>().getCoordinateList();
  const auto& detection_frame = source.getProperty<DetectionFrame>().getFrame();
  const auto weight_image = detection_frame->getWeightImage();

  auto rms = detection_frame->getBackgroundRMS();
  SeFloat background_variance = (weight_image != nullptr && detection_frame->isWeightAbsolute()) ? 1 : rms * rms;

  SeFloat total_flux = 0.0;
  SeFloat total_variance = 0.0;

  if (weight_image != nullptr) {
    auto value_iter = pixel_values.begin();;
    for (auto coord : pixel_coordinates) {
      auto value = *value_iter++;
      total_flux += value;
      total_variance += weight_image->getValue(coord.m_x, coord.m_y) * background_variance;
    }
  } else {
    for (auto value : pixel_values) {
      total_flux += value;
      total_variance += background_variance;
    }
  }

//  // Add variance from gain
//  SeFloat gain = 1; // FIXME: get gain from options or fits file
//  total_variance += total_flux / gain;

  auto flux_error = sqrt(total_variance);

  auto mag = total_flux > 0.0 ? -2.5*log10(total_flux) + m_magnitude_zero_point : SeFloat(99.0);
  auto mag_error = 1.0857 * flux_error / total_flux;

  source.setProperty<IsophotalFlux>(total_flux, flux_error , mag, mag_error);
}

}


