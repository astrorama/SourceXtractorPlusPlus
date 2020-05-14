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

#ifndef SOURCEXTRACTORPLUSPLUS_IMAGEMODE_H
#define SOURCEXTRACTORPLUSPLUS_IMAGEMODE_H

#include "SEFramework/Image/ImageBase.h"
#include "SEFramework/Image/VectorImage.h"

namespace SourceXtractor {

/**
 * Computes the background of an image using a \f$ \kappa \sigma \f$ clipping
 * individually for each cell.
 * @details
 * For each cell, pixel values are clipped to a maximum deviation of \f$ \kappa_1 \f$
 * around the mean, For the remaining pixels, and histogram ranging \f \pm \kappa_2 \sigma \f
 * is built, and iteratively clipped \f$ to \kappa_3 \sigma \f$ around the median until convergence.
 *
 * The background value for the given cell is considered to be either:
 *
 * - The mean if the standard deviation is 0
 * - 2.5 * median - 1.5 mean if the mean and the median do not diverge more than 30% (not crowded cell)
 * - The median otherwise (crowded cell)
 * @see KappaSigmaBinning
 * @tparam T
 *  Pixel type
 */
template<typename T>
class ImageMode {
public:

  /**
   * Constructor
   * @param image
   *    The image to model
   * @param cell_w
   *    Cell width
   * @param cell_h
   *    Cell height
   * @param invalid_value
   *    Pixels with this value will be discarded
   * @param kappa1
   *    First cut
   * @param kappa2
   *    Histogram range
   * @param kappa3
   *    Iterative cut
   * @param rtol
   *    Relative tolerance used to test for convergence around the median
   * @param max_iter
   *    Maximum number of iterations
   */
  ImageMode(const std::shared_ptr<Image<T>>& image, const std::shared_ptr<Image<T>>& variance,
            int cell_w, int cell_h,
            T invalid_value, T kappa1 = 2, T kappa2 = 5, T kappa3 = 3,
            T rtol = 1e-4, size_t max_iter = 100);

  /**
   * Destructor
   */
  virtual ~ImageMode() = default;

  /**
   * @return An image with the background value for each cell.
   * @note
   * Dimensions will be image->getWidth() / cell_width, ceil(image->getHeight() / cell_height)
   */
  std::shared_ptr<VectorImage<T>> getModeImage() const;

  /**
   * @return An image with the standard deviation for each cell.
   * @note
   * Dimensions will be image->getWidth() / cell_width, ceil(image->getHeight() / cell_height)
   */
  std::shared_ptr<VectorImage<T>> getSigmaImage() const;

  /**
   * @return An image with the mode of each cell of the variance map,
   *         or nullptr if there is no variance map.
   */
  std::shared_ptr<VectorImage<T>> getVarianceModeImage() const;

  /**
   * @return An image with the standard deviation of each cell from the variance map,
   *         or nullptr if there is no variance map.
   */
  std::shared_ptr<VectorImage<T>> getVarianceSigmaImage() const;

private:
  std::shared_ptr<const Image<T>> m_image;
  std::shared_ptr<VectorImage<T>> m_mode, m_sigma;
  std::shared_ptr<VectorImage<T>> m_var_mode, m_var_sigma;
  int m_cell_w, m_cell_h;
  T m_invalid, m_kappa1, m_kappa2, m_kappa3, m_rtol;
  size_t m_max_iter;

  std::tuple<T, T> getBackGuess(const std::vector<T> &data) const;
  void processCell(const Image<T>& img, int x, int y, VectorImage<T>& out_mode, VectorImage<T>& out_sigma) const;
};

extern template
class ImageMode<SeFloat>;

} // end of namespace SourceXtractor

#endif //SOURCEXTRACTORPLUSPLUS_IMAGEMODE_H
