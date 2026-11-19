/** Copyright © 2019-2026 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
#include <algorithm>
#include <cmath>
#include <limits>

#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/CompactExponentialModel.h"
#include "ModelFitting/Models/FrameModel.h"
#include "ModelFitting/Parameters/ManualParameter.h"
#include "SEImplementation/Image/ImageInterfaceTraits.h"
#include "SEImplementation/Plugin/SpreadModel/SpreadModelUtils.h"
#include "SEUtils/IsNan.h"

namespace SourceXtractor {
namespace {
bool finite(double value) {
  // Ordinary isnan/isinf checks may be optimized away under -ffast-math.
  return !fastmath_isnan(value) && !fastmath_isinf(value);
}
SpreadModel invalidResult() {
  const auto nan = std::numeric_limits<SeFloat>::quiet_NaN();
  return SpreadModel(nan, nan);
}

// Validate the entire kernel while locating the first global maximum.
std::pair<double, std::size_t> findPsfPeak(const VectorImage<SeFloat>& kernel) {
  const auto& pixels = kernel.getData();
  double peak = 0;
  std::size_t peak_index = 0;
  for (std::size_t i = 0; i < pixels.size(); ++i) {
    if (!finite(pixels[i])) {
      return {std::numeric_limits<double>::quiet_NaN(), 0};
    }
    if (pixels[i] > peak) {
      peak = pixels[i];
      peak_index = i;
    }
  }
  return {peak, peak_index};
}

double interpolatePsf(const VectorImage<SeFloat>& kernel, std::size_t x, std::size_t y,
                      unsigned int subdivisions) {
  const int ix = std::min<int>(x / subdivisions, kernel.getWidth() - 2);
  const int iy = std::min<int>(y / subdivisions, kernel.getHeight() - 2);
  const double dx = double(x) / subdivisions - ix;
  const double dy = double(y) / subdivisions - iy;
  return (1 - dy) * ((1 - dx) * kernel.getValue(ix, iy) + dx * kernel.getValue(ix + 1, iy)) +
         dy * ((1 - dx) * kernel.getValue(ix, iy + 1) + dx * kernel.getValue(ix + 1, iy + 1));
}

// Return the connected half-maximum area in original kernel pixels squared.
double halfMaximumArea(const VectorImage<SeFloat>& kernel, double peak, std::size_t peak_index,
                       unsigned int subdivisions) {
  const auto nan = std::numeric_limits<double>::quiet_NaN();
  // Sample between the original kernel pixel centres. Flood-fill only the
  // four-connected half-maximum region of the global peak, excluding detached lobes.
  const std::size_t nx = std::size_t(kernel.getWidth() - 1) * subdivisions + 1;
  const std::size_t ny = std::size_t(kernel.getHeight() - 1) * subdivisions + 1;
  std::vector<bool> visited(nx * ny, false);
  std::vector<std::size_t> pending{(peak_index / kernel.getWidth()) * subdivisions * nx +
                                   (peak_index % kernel.getWidth()) * subdivisions};
  std::size_t area_samples = 0;
  while (!pending.empty()) {
    const auto index = pending.back();
    pending.pop_back();
    if (visited[index]) {
      continue;
    }
    visited[index] = true;
    const auto x = index % nx;
    const auto y = index / nx;
    const double value = interpolatePsf(kernel, x, y, subdivisions);
    if (value < peak / 2) {
      continue;
    }
    if (x == 0 || y == 0 || x == nx - 1 || y == ny - 1) {
      // The half-maximum contour is not enclosed, so its area is unknown.
      return nan;
    }
    ++area_samples;
    pending.insert(pending.end(), {index - 1, index + 1, index - nx, index + nx});
  }
  return double(area_samples) / (double(subdivisions) * subdivisions);
}

}

double SpreadModelUtils::computePsfFwhm(const VectorImage<SeFloat>& kernel, double pixel_sampling,
                                      unsigned int subdivisions) {
  const auto nan = std::numeric_limits<double>::quiet_NaN();
  if (!finite(pixel_sampling) || pixel_sampling <= 0 || subdivisions == 0 ||
      kernel.getWidth() < 3 || kernel.getHeight() < 3) {
    return nan;
  }
  const auto peak = findPsfPeak(kernel);
  if (!finite(peak.first) || peak.first <= 0) {
    return nan;
  }
  // Area is in original kernel pixels squared. The equivalent-circle diameter
  // defines our FWHM for non-circular PSFs; pixel_sampling converts it to image pixels.
  const double area = halfMaximumArea(kernel, peak.first, peak.second, subdivisions);
  const double fwhm = 2 * std::sqrt(area / M_PI) * pixel_sampling;
  return finite(fwhm) && fwhm > 0 ? fwhm : nan;
}

std::pair<std::shared_ptr<VectorImage<SeFloat>>, std::shared_ptr<VectorImage<SeFloat>>>
SpreadModelUtils::renderModels(double x, double y, double flux, double fwhm, int width, int height,
                              const DownSampledImagePsf& psf) {
  using namespace ModelFitting;
  using ImageType = std::shared_ptr<VectorImage<SeFloat>>;
  // Freeze the point fit's position and flux: the exponential is rendered, never fitted.
  auto parameter = [](double value) { return std::make_shared<ManualParameter>(value); };
  auto px = parameter(x);
  auto py = parameter(y);
  auto amplitude = parameter(flux);
  std::vector<PointModel> points;
  points.emplace_back(px, py, amplitude);
  FrameModel<DownSampledImagePsf, ImageType> point_model(1.0, width, height, {}, std::move(points), {}, psf);
  auto point_stamp = point_model.getImage();

  const double h = fwhm / 16;
  // Match the flexible exponential renderer's extent and central adaptive sampling.
  const int size = std::max(4.0, 1.2 * std::max(width, height));
  std::vector<std::shared_ptr<ExtendedModel<ImageType>>> extended;
  // I(r) = I0 exp(-r/h), whose total flux is 2*pi*I0*h^2. Unit axis scales,
  // zero rotation and an identity transform make it circular in measurement pixels.
  extended.emplace_back(std::make_shared<CompactExponentialModel<ImageType>>(
      2.0, parameter(flux / (2 * M_PI * h * h)), parameter(1 / h), parameter(1), parameter(1),
      parameter(0), size, size, px, py, amplitude, std::make_tuple(1.0, 0.0, 0.0, 1.0)));
  FrameModel<DownSampledImagePsf, ImageType> exponential_model(1.0, width, height, {}, {}, std::move(extended), psf);
  // Both templates use the same PSF normalization and sampling. Keep their cropped
  // fluxes: independently renormalizing the stamps would change the statistic.
  return {point_stamp, exponential_model.getImage()};
}

SpreadModel SpreadModelUtils::computeSpread(const VectorImage<SeFloat>& point,
                                           const VectorImage<SeFloat>& exponential,
                                           const VectorImage<SeFloat>& data,
                                           const std::vector<double>& variance,
                                           const std::vector<bool>& valid) {
  const auto& p = data.getData();
  const auto& phi = point.getData();
  const auto& g = exponential.getData();
  if (point.getWidth() != data.getWidth() || point.getHeight() != data.getHeight() ||
      exponential.getWidth() != data.getWidth() || exponential.getHeight() != data.getHeight() ||
      variance.size() != p.size() || valid.size() != p.size()) {
    return invalidResult();
  }
  auto usable = [&](std::size_t i) {
    return valid[i] && finite(p[i]) && finite(phi[i]) && finite(g[i]) &&
           finite(variance[i]) && variance[i] > 0;
  };
  // Spread uses uniform weights on usable pixels, unlike the inverse-variance
  // point fit. Variances enter only the propagated uncertainty below.
  // a = phi.p, b = G.p, c = G.phi, d = phi.phi, all over the same mask.
  double a = 0, b = 0, c = 0, d = 0;
  for (std::size_t i = 0; i < p.size(); ++i) {
    if (usable(i)) {
      a += double(phi[i]) * p[i];
      b += double(g[i]) * p[i];
      c += double(g[i]) * phi[i];
      d += double(phi[i]) * phi[i];
    }
  }
  if (!finite(a) || !finite(b) || !finite(c) || !finite(d) || a <= 0 || d <= 0) {
    return invalidResult();
  }
  // Subtract the templates' own response so data proportional to phi gives zero.
  const double spread = b / a - c / d;
  // Propagate independent pixel noise with both templates held fixed. This does
  // not propagate fitted-centroid or PSF uncertainty. Summing squared derivatives
  // avoids cancellation in the equivalent expanded variance formula.
  double error_squared = 0;
  for (std::size_t i = 0; i < p.size(); ++i) {
    if (usable(i)) {
      // Algebraically (a*g - b*phi)/a^2, avoiding intermediate fourth powers of a.
      const double derivative = (g[i] - (b / a) * phi[i]) / a;
      error_squared += variance[i] * derivative * derivative;
    }
  }
  const SeFloat value = spread;
  const SeFloat error = std::sqrt(error_squared);
  if (!finite(value) || !finite(error)) {
    return invalidResult();
  }
  return SpreadModel(value, error);
}
} // namespace SourceXtractor
