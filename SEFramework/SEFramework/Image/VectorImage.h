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
/**
 * @file SEFramework/Image/VectorImage.h
 * @date 06/14/16
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_IMAGE_VECTORIMAGE_H
#define _SEFRAMEWORK_IMAGE_VECTORIMAGE_H

#include <vector>
#include <cassert>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/WriteableImage.h"

#include "SEFramework/Image/ImageChunk.h"
#include "SEFramework/Image/ImageBase.h"


// FIXME get rid of offset, it was a bad idea

namespace SExtractor {

/**
 * @class VectorImage
 * @brief Image implementation which keeps the pixel values in memory
 * 
 * @details
 * The pixel values are kept in a vector. The first element of the vector is
 * the pixel with coordinate (0,0) and the following elements have increasing
 * x and y indices, with the x index (in the width direction) increasing more
 * rapidly. This means the order of the vector elements should be:
 * 
 * (0,0), (1,0), (2,0), ..., (width-1,0), (0,1), (1,1), ..., (width-1,height-1)
 * 
 * @tparam T the type of the pixel values
 */
template <typename T>
class VectorImage final : public ImageBase<T>, public WriteableImage<T> {
protected:

  VectorImage(int width, int height) : m_width(width), m_height(height), m_data(width * height), m_offset(0,0) {
    assert(width > 0 && height > 0);
  }

  VectorImage(int width, int height, PixelCoordinate offset) :
    m_width(width), m_height(height), m_data(width * height), m_offset(offset) {
    assert(width > 0 && height > 0);
  }

  VectorImage(int width, int height, std::vector<T> data) :
      m_width(width), m_height(height), m_data(std::move(data)), m_offset(0, 0) {
    assert(width > 0 && height > 0);
    assert(m_data.size() == std::size_t(width * height));
  }

  template <typename Iter>
  VectorImage(int width, int height, Iter data_begin, Iter data_end,
        typename std::enable_if<
            std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<Iter>::iterator_category>::value
            and std::is_same<T, typename std::iterator_traits<Iter>::value_type>::value
        >::type* =0) :
      m_width(width), m_height(height), m_data(data_begin, data_end) {
    assert(m_data.size() == std::size_t(width * height));
  }
  
  VectorImage(const Image<T>& other_image) :
    m_width(other_image.getWidth()), m_height(other_image.getHeight()), m_data(m_width * m_height), m_offset(0,0) {
    for (int y = 0; y < m_height; y++) {
      for (int x = 0; x < m_width; x++) {
        setValue(x, y, other_image.getValue(x, y));
      }
    }
  }

  VectorImage(const std::shared_ptr<const Image<T>>& other_image): VectorImage(static_cast<const Image<T>&>(*other_image)) {}

public:
  template<typename... Args>
  static std::shared_ptr<VectorImage<T>> create(Args&&... args) {
    return std::shared_ptr<VectorImage<T>>(new VectorImage<T>(std::forward<Args>(args)...));
  }

  std::string getRepr() const override {
    return "VectorImage<" + std::to_string(m_width) + "," + std::to_string(m_height) + ">";
  }

  int getHeight() const override {
    return m_height;
  }
  
  int getWidth() const override {
    return m_width;
  }

  using Image<T>::getValue;
  T getValue(int x, int y) const override {
    return const_cast<VectorImage<T>*>(this)->at(x, y);
  }
  
  void setValue(int x, int y, T value) override {
    at(x,y) = value;
  }

  void setValue(PixelCoordinate pc, T value) {
    setValue(pc.m_x, pc.m_y, value);
  }

  T& at(int x, int y) {
    x = x - m_offset.m_x;
    y = y - m_offset.m_y;

    assert(x >= 0 && y >=0 && x < m_width && y < m_height);
    return m_data[x + y * m_width];
  }

  void fillValue(T value) {
    std::fill(m_data.begin(), m_data.end(), value);
  }

  const std::vector<T>& getData() const {
    return m_data;
  }

  std::vector<T>& getData() {
    return m_data;
  }

  PixelCoordinate getOffset() const {
    return m_offset;
  }

  /**
   * @brief Destructor
   */
  virtual ~VectorImage() = default;

  virtual std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const override {
    return ImageChunk<T>::create(&m_data[x + y * m_width], width, height, m_width, this->shared_from_this());
  }


private:

  int m_width;
  int m_height;
  std::vector<T> m_data;
  PixelCoordinate m_offset;

}; /* End of VectorImage class */

} /* namespace SExtractor */


#endif
