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

#ifndef _SEFRAMEWORK_IMAGE_PROCESSEDIMAGE_H_
#define _SEFRAMEWORK_IMAGE_PROCESSEDIMAGE_H_

#include <memory>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Image/VectorImage.h"

namespace SourceXtractor {

/**
 * @class ProcessedImage
 * @brief Processes two images to create a third combining them by using any function
 *
 */

template <typename T, typename P>
class ProcessedImage : public Image<T> {

protected:

  ProcessedImage(std::shared_ptr<const Image<T>> image_a, std::shared_ptr<const Image<T>> image_b)
      : m_image_a(image_a), m_image_b(image_b) {
    assert(m_image_a->getWidth() == m_image_b->getWidth());
    assert(m_image_a->getHeight() == m_image_b->getHeight());
  };

public:

  /**
   * @brief Destructor
   */
  virtual ~ProcessedImage() = default;

  static std::shared_ptr<ProcessedImage<T, P>> create(
      std::shared_ptr<const Image<T>> image_a, std::shared_ptr<const Image<T>> image_b) {
    return std::shared_ptr<ProcessedImage<T, P>>(new ProcessedImage<T, P>(image_a, image_b));
  }

  static std::shared_ptr<ProcessedImage<T, P>> create(std::shared_ptr<const Image<T>> image_a, T value) {
    return std::shared_ptr<ProcessedImage<T, P>>(new ProcessedImage<T, P>(
        image_a, ConstantImage<T>::create(image_a->getWidth(), image_a->getHeight(), value)));
  }

  std::string getRepr() const override {
    return "ProcessedImage(" + m_image_a->getRepr() + "," + m_image_b->getRepr() + ")";
  }

  int getWidth() const override {
    return m_image_a->getWidth();
  }

  int getHeight() const override {
    return m_image_a->getHeight();
  }

  std::shared_ptr<VectorImage<T>> getChunk(int x, int y, int width, int height) const override {
    std::vector<T> new_chunk_data(width * height);
    auto a_chunk = m_image_a->getChunk(x, y, width, height);
    auto b_chunk = m_image_b->getChunk(x, y, width, height);
    for (int iy = 0; iy < height; ++iy) {
      for (int ix = 0; ix < width; ++ix) {
        new_chunk_data[ix + iy * width] = P::process(a_chunk->getValue(ix, iy),
                                                     b_chunk->getValue(ix, iy));
      }
    }
    return VectorImage<T>::create(width, height, std::move(new_chunk_data));
  }

private:
  std::shared_ptr<const Image<T>> m_image_a;
  std::shared_ptr<const Image<T>> m_image_b;

}; /* End of ProcessedImage class */


template<typename T>
struct SubtractOperation
{
  static T process(const T& a, const T& b) { return a - b; }
};

template<typename T>
using SubtractImage = ProcessedImage<T, SubtractOperation<T>> ;

template<typename T>
struct MultiplyOperation
{
  static T process(const T& a, const T& b) { return a * b; }
};

template<typename T>
using MultiplyImage = ProcessedImage<T, MultiplyOperation<T>> ;

template<typename T>
struct SnrOperation
{
  static T process(const T& a, const T& b) { return a / sqrt(b); }
};

template<typename T>
using SnrImage = ProcessedImage<T, SnrOperation<T>> ;

} /* namespace SourceXtractor */




#endif /* _SEFRAMEWORK_IMAGE_PROCESSEDIMAGE_H_ */
