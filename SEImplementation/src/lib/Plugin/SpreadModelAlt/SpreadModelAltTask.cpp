/** Copyright © 2019-2026 Université de Genève, LMU Munich - Faculty of Physics,
 * IAP-CNRS/Sorbonne Université
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
#include "SEImplementation/Plugin/SpreadModelAlt/SpreadModelAltTask.h"
#include "SpreadModelAltFit.h"

namespace SourceXtractor {

using namespace ModelFitting;
using Euclid::make_unique;

using namespace SpreadModelAltFit;

double SpreadModelAltTask::computeDownScaling(SourceInterface& source) const {
  // Match iterative fitting: estimate the largest stamp area in PSF pixels
  // and use one rendering scale for all measurement frames.
  double fit_size = 0;
  for (auto frame_index : m_measurement_frames) {
    const auto &frame_rect =
        source.getProperty<MeasurementFrameRectangle>(frame_index);
    if (!frame_rect.isValid() || frame_rect.isEmpty()) {
      continue;
    }
    const auto &psf_property =
        source.getProperty<SourcePsfProperty>(frame_index);
    if (!psf_property.getPsf()) {
      continue;
    }
    const auto sampling = psf_property.getPixelSampling();
    fit_size =
        std::max(fit_size, frame_rect.getWidth() * frame_rect.getHeight() /
                               (sampling * sampling));
  }

  double down_scaling = m_scale_factor;
  if (fit_size > 2.0 * m_max_fit_area) {
    down_scaling *= std::sqrt(m_max_fit_area / fit_size);
  }

  return down_scaling;
}

SpreadModelAlt SpreadModelAltTask::computeFrameSpread(SourceInterface& source, double down_scaling) const {
  const auto& frame_rect = source.getProperty<MeasurementFrameRectangle>(m_instance);
  if (!frame_rect.isValid() || frame_rect.isEmpty()) {
    return invalidResult();
  }
  // Point-source rendering requires a PSF.
  const auto& psf_property = source.getProperty<SourcePsfProperty>(m_instance);
  if (!psf_property.getPsf()) {
    return invalidResult();
  }
  // Measure the original local PSF before any rendering downsampling changes its profile.
  const double fwhm = SpreadModelAltUtils::computePsfFwhm(*psf_property.getPsf(), psf_property.getPixelSampling());
  if (!finite(fwhm) || fwhm <= 0) {
    return invalidResult();
  }
  const auto frame_coordinates = source.getProperty<MeasurementFrameCoordinates>(m_instance).getCoordinateSystem();
  const auto& frame_images = source.getProperty<MeasurementFrameImages>(m_instance);
  const auto& frame_info = source.getProperty<MeasurementFrameInfo>(m_instance);
  const PixelCoordinate offset(frame_rect.getTopLeft().m_x, frame_rect.getTopLeft().m_y);
  const auto width = std::size_t(frame_rect.getWidth());
  const auto height = std::size_t(frame_rect.getHeight());
  if (width == 0 || height == 0) {
    return invalidResult();
  }
  auto stamp = prepareFitStamp(frame_images, frame_info, offset, width, height);
  const auto& centroid = source.getProperty<PixelCentroid>();
  const auto iso_flux = source.getProperty<IsophotalFlux>().getFlux();
  const double guess_x = centroid.getCentroidX();
  const double guess_y = centroid.getCentroidY();
  // The point fit has three free parameters (x, y, flux); require at least
  // one residual degree of freedom after masking.
  if (stamp.valid_pixels < 4 || !finite(iso_flux) || !finite(guess_x) || !finite(guess_y)) {
    return invalidResult();
  }
  // FIXME: temporary hardcoded floor, matching PythonConfig/ObjectInfo.cpp.
  // Keep the initial flux positive for ExpSigmoidConverter; this should be configurable.
  const auto flux_guess = std::max<double>(iso_flux, 0.0001);
  SourceModel source_model(flux_guess, guess_x, guess_y, 5.0 /* FIXME radius_guess * 2 */);
  const auto reference_coordinates = source.getProperty<ReferenceCoordinates>().getCoordinateSystem();
  auto point = source_model.createPointModelForFrame(reference_coordinates, frame_coordinates, offset);
  if (!centreInFrame(point, offset, frame_info)) {
    return invalidResult();
  }
  DownSampledImagePsf psf(psf_property.getPixelSampling(), psf_property.getPsf(), down_scaling,
                          m_should_renormalize.at(m_instance));
  if (!fitPointSource(source_model, std::move(point), stamp, psf, m_least_squares_engine, m_max_iterations)) {
    return invalidResult();
  }
  // Snapshot the fitted position in exactly the same pixel convention as the point fit.
  const auto fitted_point = source_model.createPointModelForFrame(reference_coordinates, frame_coordinates, offset);
  const auto flux = source_model.flux->getValue();
  if (!finite(flux) || flux <= 0 || !centreInFrame(fitted_point, offset, frame_info)) {
    return invalidResult();
  }
  const auto stamps = SpreadModelAltUtils::renderModels(fitted_point.getX(), fitted_point.getY(), flux, fwhm,
                                                    width, height, psf);
  return SpreadModelAltUtils::computeSpread(*stamps.first, *stamps.second, *stamp.image, stamp.variances, stamp.valid);
}

void SpreadModelAltTask::computeProperties(SourceInterface& source) const {
  const auto result = computeFrameSpread(source, computeDownScaling(source));
  source.setIndexedProperty<SpreadModelAlt>(m_instance, result.getSpreadModelAlt(), result.getSpreadModelAltError());
}

} // namespace SourceXtractor
