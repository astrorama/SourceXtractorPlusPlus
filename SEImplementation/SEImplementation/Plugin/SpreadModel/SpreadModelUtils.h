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
#ifndef _SEIMPLEMENTATION_PLUGIN_SPREADMODEL_SPREADMODELUTILS_H_
#define _SEIMPLEMENTATION_PLUGIN_SPREADMODEL_SPREADMODELUTILS_H_

#include <utility>
#include <vector>
#include "SEImplementation/Image/DownSampledImagePsf.h"
#include "SEImplementation/Plugin/SpreadModel/SpreadModel.h"

namespace SourceXtractor {

/** Numerical operations used by the per-frame spread-model measurement. */
class SpreadModelUtils {
public:
  /** Equivalent-area half-maximum diameter in image pixels; NaN for invalid/truncated kernels.
   * Pass the original local kernel, before rendering downsampling. pixel_sampling is
   * the size of a kernel pixel in measurement-image pixels. Only the connected region
   * containing the maximum contributes to the half-maximum area.
   * subdivisions controls bilinear sampling of each original PSF pixel interval.
   */
  static double computePsfFwhm(const VectorImage<SeFloat>& kernel, double pixel_sampling,
                              unsigned int subdivisions = 16);

  /** Render point and circular exponential stamps at the same stamp-local position and flux.
   * fwhm and position are in measurement-image pixels, with pixel centers at half-integers.
   * The exponential scale length is fwhm/16; no cropped-stamp renormalization is performed.
   */
  static std::pair<std::shared_ptr<VectorImage<SeFloat>>, std::shared_ptr<VectorImage<SeFloat>>>
  renderModels(double x, double y, double flux, double fwhm, int width, int height,
               const DownSampledImagePsf& psf);

  /** Compute spread and fixed-template pixel-noise uncertainty using a binary validity mask.
   * Usable pixels have uniform weight; variance is used only for uncertainty propagation.
   * Fit sufficiency and coverage checks belong to the caller, not this algebraic helper.
   * Variances are in squared image units. Invalid inputs/denominators produce NaN results.
   */
  static SpreadModel computeSpread(const VectorImage<SeFloat>& point, const VectorImage<SeFloat>& exponential,
                                  const VectorImage<SeFloat>& data, const std::vector<double>& variance,
                                  const std::vector<bool>& valid);
};

} // namespace SourceXtractor
#endif
