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

#include "SEFramework/Aperture/CircularAperture.h"
#include "SEImplementation/Measurement/MultithreadedMeasurement.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurve.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurveTask.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "SEImplementation/Plugin/KronRadius/KronRadius.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEUtils/Mat22.h"


namespace SourceXtractor {

using SExtractor::Mat22;

static const SeFloat GROWTH_NSIG = 6.;
static const size_t GROWTH_NSAMPLES = 64;

GrowthCurveTask::GrowthCurveTask(unsigned instance, bool use_symmetry)
  : m_instance{instance}, m_use_symmetry{use_symmetry} {}

void GrowthCurveTask::computeProperties(SourceInterface& source) const {
  // Measurement frame and properties defined on it
  auto measurement_frame = source.getProperty<MeasurementFrame>(m_instance).getFrame();
  auto image = measurement_frame->getSubtractedImage();
  auto variance_map = measurement_frame->getVarianceMap();
  auto variance_threshold = measurement_frame->getVarianceThreshold();
  auto centroid_x = source.getProperty<MeasurementFramePixelCentroid>(m_instance).getCentroidX();
  auto centroid_y = source.getProperty<MeasurementFramePixelCentroid>(m_instance).getCentroidY();
  Mat22 jacobian{source.getProperty<JacobianSource>(m_instance).asTuple()};

  // These are from the detection frame
  auto& shape = source.getProperty<ShapeParameters>();
  auto& kron = source.getProperty<KronRadius>();

  // Radius for computing the growth curve and step size *on the detection frame*
  double detection_rlim = std::max(GROWTH_NSIG * shape.getEllipseA(), kron.getKronRadius());

  // Now we need to compute the rlim for the measurement frame
  // We take two vectors defined by the radius on the detection frame along the X and Y,
  // transform them, and we use as a limit now the longest of the two after the transformation
  Mat22 radius_22{detection_rlim, 0, 0, detection_rlim};
  radius_22 = radius_22 * jacobian;
  double r1 = radius_22[0] * radius_22[0] + radius_22[1] * radius_22[1];
  double r2 = radius_22[2] * radius_22[2] + radius_22[3] * radius_22[3];
  double rlim = std::sqrt(std::max(r1, r2));

  double step_size = rlim / GROWTH_NSAMPLES;

  // List of apertures
  std::vector<CircularAperture> apertures;
  std::vector<double> fluxes(GROWTH_NSAMPLES);
  apertures.reserve(GROWTH_NSAMPLES);
  for (size_t step = 1; step <= GROWTH_NSAMPLES; ++step) {
    apertures.emplace_back(step_size * step);
  }

  // Boundaries for the computation
  // We know the last aperture is the widest, so we take the limits from it
  auto min_coord = apertures.back().getMinPixel(centroid_x, centroid_y);
  auto max_coord = apertures.back().getMaxPixel(centroid_x, centroid_y);

  // Compute fluxes for each ring
  std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);
  for (auto y = min_coord.m_y; y <= max_coord.m_y; ++y) {
    for (auto x = min_coord.m_x; x <= max_coord.m_x; ++x) {
      if (x >= 0 && x < image->getWidth() && y >= 0 && y < image->getHeight()) {
        // Get the pixel value
        WeightImage::PixelType variance_tmp = variance_map ? variance_map->getValue(x, y) : 1;
        DetectionImage::PixelType pixel_value = 0;
        // Masked out
        if (variance_tmp > variance_threshold) {
          if (m_use_symmetry) {
            auto mirror_x = 2 * centroid_x - x + 0.49999;
            auto mirror_y = 2 * centroid_y - y + 0.49999;
            if (mirror_x >= 0 && mirror_y >= 0 && mirror_x < image->getWidth() && mirror_y < image->getHeight()) {
              variance_tmp = variance_map ? variance_map->getValue(mirror_x, mirror_y) : 1;
              if (variance_tmp < variance_threshold) {
                // mirror pixel is OK: take the value
                pixel_value = image->getValue(mirror_x, mirror_y);
              }
            }
          }
        }
        // Not masked
        else {
          pixel_value = image->getValue(x, y);
        }

        // Assign the pixel value according to the affected area
        auto dx = x - centroid_x;
        auto dy = y - centroid_y;
        double r = std::sqrt(dx * dx + dy * dy);
        auto aperture_idx = static_cast<size_t>(r / step_size);
        // Consider previous and following rings
        size_t idx = 0;
        if (aperture_idx > 0)
          idx = aperture_idx - 1;
        double inner = 0;
        for (; idx <= aperture_idx + 1; ++idx) {
          if (idx < apertures.size()) {
            auto& aperture = apertures[idx];
            auto area = aperture.getArea(centroid_x, centroid_y, x, y);

            fluxes[idx] += area * pixel_value - inner;
            inner = area * pixel_value;
          }
        }
      }
    }
  }

  // Accumulate
  for (size_t i = 1; i < fluxes.size(); ++i) {
    fluxes[i] += fluxes[i - 1];
  }

  // Set property
  source.setIndexedProperty<GrowthCurve>(m_instance, std::move(fluxes), rlim);
}

} // end of namespace SourceXtractor
