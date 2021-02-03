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
 * ThresholdedImage.h
 *
 *  Created on: Jan 10, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_THRESHOLDEDIMAGE_H_
#define _SEFRAMEWORK_IMAGE_THRESHOLDEDIMAGE_H_

#include <memory>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageChunk.h"
#include "SEFramework/Image/ConstantImage.h"

namespace SourceXtractor {

/**
 * @class ThresholdedImage
 * @brief Used to subtract a constant value from an Image
 *
 */
template <typename T>
class ThresholdedImage : public Image<T> {

protected:

  ThresholdedImage(std::shared_ptr<const Image<T>> image, std::shared_ptr<const Image<T>> variance_map, T threshold_multiplier)
      : m_image(image), m_variance_map(variance_map), m_threshold_multiplier(threshold_multiplier) {
    assert(m_image->getWidth() == m_variance_map->getWidth());
    assert(m_image->getHeight() == m_variance_map->getHeight());
  };

public:

  /**
   * @brief Destructor
   */
  virtual ~ThresholdedImage() = default;

  static std::shared_ptr<ThresholdedImage<T>> create(
      std::shared_ptr<const Image<T>> image, std::shared_ptr<const Image<T>> variance_map, T threshold_multiplier) {
    return std::shared_ptr<ThresholdedImage<T>>(new ThresholdedImage<T>(image, variance_map, threshold_multiplier));
  }

  std::string getRepr() const override {
    return "ThresholdedImage(" + m_image->getRepr() + ")";
  }

  int getWidth() const override {
    return m_image->getWidth();
  }

  int getHeight() const override {
    return m_image->getHeight();
  }

  std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const override{
    auto img_chunk = m_image->getChunk(x, y, width, height);
    auto var_chunk = m_variance_map->getChunk(x, y, width, height);
    auto chunk = UniversalImageChunk<T>::create(std::move(*img_chunk));
    for (int iy = 0; iy < height; ++iy) {
      for (int ix = 0; ix < width; ++ix) {
        chunk->at(ix, iy) -= sqrt(var_chunk->getValue(ix, iy)) * m_threshold_multiplier;
      }
    }
    return chunk;
  }

private:
  std::shared_ptr<const Image<T>> m_image, m_variance_map;
  T m_threshold_multiplier;

}; /* End of ThresholdedImage class */

} /* namespace SourceXtractor */

#endif /* _SEFRAMEWORK_IMAGE_THRESHOLDEDIMAGE_H_ */
