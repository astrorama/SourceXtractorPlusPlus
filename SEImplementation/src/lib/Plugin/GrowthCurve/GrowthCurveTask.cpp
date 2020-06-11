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
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Measurement/MultithreadedMeasurement.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurve.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurveTask.h"
#include "SEImplementation/Plugin/KronRadius/KronRadius.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"


namespace SourceXtractor {

static const SeFloat GROWTH_NSIG = 6.;
static const size_t GROWTH_NSAMPLES = 64;

GrowthCurveTask::GrowthCurveTask(bool use_symmetry): m_use_symmetry{use_symmetry} {}

void GrowthCurveTask::computeProperties(SourceInterface& source) const {
  auto detection_frame = source.getProperty<DetectionFrame>().getFrame();
  auto image = detection_frame->getSubtractedImage();
  auto variance_map = detection_frame->getVarianceMap();
  auto variance_threshold = detection_frame->getVarianceThreshold();
  auto& shape = source.getProperty<ShapeParameters>();
  auto& kron = source.getProperty<KronRadius>();
  auto centroid_x = source.getProperty<PixelCentroid>().getCentroidX();
  auto centroid_y = source.getProperty<PixelCentroid>().getCentroidY();

  // Radius for computing the growth curve and step size
  double rlim = std::max(GROWTH_NSIG * shape.getEllipseA(), kron.getKronRadius());
  double step_size = rlim / GROWTH_NSAMPLES;

  // List of apertures
  std::vector<CircularAperture> apertures;
  std::vector<double> fluxes(GROWTH_NSAMPLES);
  apertures.reserve(GROWTH_NSAMPLES);
  for (size_t step = 1; step <= GROWTH_NSAMPLES; ++step) {
    apertures.emplace_back(step_size * step);
  }

  // Boundaries for the computation
  PixelCoordinate min_coord{static_cast<int>(centroid_x - rlim), static_cast<int>(centroid_y - rlim)};
  PixelCoordinate max_coord{static_cast<int>(centroid_x + rlim + 1), static_cast<int>(centroid_y + rlim + 1)};

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
  source.setProperty<GrowthCurve>(std::move(fluxes), rlim);
}

} // end of namespace SourceXtractor
