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

#ifndef SOURCEXTRACTORPLUSPLUS_REPLACEUNDEFIMAGE_H
#define SOURCEXTRACTORPLUSPLUS_REPLACEUNDEFIMAGE_H

#include "SEFramework/Image/VectorImage.h"

namespace SourceXtractor {

/**
 * Replace undefined (i.e. masked) values with the average of the closest defined pixel values
 * @tparam T
 *  Pixel type
 */
template <typename T>
class ReplaceUndefImage: public ImageBase<T> {
private:
  ReplaceUndefImage(const std::shared_ptr<VectorImage<T>>& image, T invalid);

public:
  /**
   * Dstructor
   */
  ~ReplaceUndefImage() = default;

  /**
   * Create an image where filtered values are replaced with the average of the closest neighbors
   * @param image
   *    Image to "fix"
   * @param undefined
   *    Value used as mask
   * @return
   *    A new image
   */
  static std::shared_ptr<ReplaceUndefImage<T>> create(const std::shared_ptr<VectorImage<T>>& image, T undefined) {
    return std::shared_ptr<ReplaceUndefImage<T>>(new ReplaceUndefImage(image, undefined));
  };

  std::string getRepr() const final;

  T getValue(int x, int y) const final;

  int getWidth() const final;

  int getHeight() const final;

private:
  std::shared_ptr<VectorImage<T>> m_image;
  T m_invalid;
};

extern template class ReplaceUndefImage<SeFloat>;

} // end of namespace SourceXtractor

#endif // SOURCEXTRACTORPLUSPLUS_REPLACEUNDEFIMAGE_H
