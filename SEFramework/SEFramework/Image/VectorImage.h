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

#include <cassert>
#include <vector>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/WriteableImage.h"

namespace SourceXtractor {

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
class VectorImage final : public WriteableImage<T> {
protected:
  VectorImage(const VectorImage<T>& other)
      : m_width(other.m_width)
      , m_height(other.m_height)
      , m_offset(0)
      , m_stride(m_width)
      , m_data(std::make_shared<std::vector<T>>(*other.m_data)) {}

  VectorImage(VectorImage<T>&& other) = default;

  VectorImage(int width, int height)
      : m_width(width), m_height(height), m_offset(0), m_stride(m_width), m_data(std::make_shared<std::vector<T>>(width * height)) {
    assert(width > 0 && height > 0);
  }

  VectorImage(int width, int height, std::vector<T>&& data)
      : m_width(width)
      , m_height(height)
      , m_offset(0)
      , m_stride(m_width)
      , m_data(std::make_shared<std::vector<T>>(std::move(data))) {
    assert(width > 0 && height > 0);
    assert(m_data->size() == std::size_t(width * height));
  }

  VectorImage(int width, int height, const std::vector<T>& data) : VectorImage(width, height, std::move(std::vector<T>(data))) {}

  template <typename Iter>
  VectorImage(int width, int height, Iter data_begin, Iter data_end,
              typename std::enable_if<
                  std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<Iter>::iterator_category>::value and
                  std::is_same<T, typename std::iterator_traits<Iter>::value_type>::value>::type* = 0)
      : m_width(width)
      , m_height(height)
      , m_offset(0)
      , m_stride(m_width)
      , m_data(std::make_shared<std::vector<T>>(data_begin, data_end)) {
    assert(m_data->size() == std::size_t(width * height));
  }

  VectorImage(const Image<T>& other) : VectorImage(std::move(*other.getChunk(0, 0, other.getWidth(), other.getHeight()))) {}

  VectorImage(const std::shared_ptr<const Image<T>>& other) : VectorImage(static_cast<const Image<T>&>(*other)) {}

  VectorImage(int width, int height, std::shared_ptr<std::vector<T>> data, int offset, int stride)
      : m_width(width), m_height(height), m_offset(offset), m_stride(stride), m_data(std::move(data)) {
    assert(static_cast<size_t>((m_height - 1) * m_stride + m_offset) <= m_data->size());
  }

public:
  template <typename IT>
  class iterator_impl : public std::iterator<std::random_access_iterator_tag, IT> {
  public:
    iterator_impl(const iterator_impl&) = default;
    iterator_impl(iterator_impl&&)      = default;

    iterator_impl& operator=(const iterator_impl&) = default;

    IT& operator*() {
      return m_img->at(m_x, m_y);
    }

    bool operator==(const iterator_impl& other) const {
      return m_img == other.m_img && m_x == other.m_x && m_y == other.m_y;
    }

    bool operator!=(const iterator_impl& other) const {
      return !(*this == other);
    }

    iterator_impl& operator++() {
      ++m_x;
      if (m_x >= m_img->m_width) {
        m_x = 0;
        ++m_y;
      }
      return *this;
    }

    iterator_impl& operator--() {
      --m_x;
      if (m_x < 0) {
        m_x = m_img->m_width - 1;
        --m_y;
      }
      return *this;
    }

    iterator_impl operator++(int) {
      iterator_impl iter(*this);
      ++(*this);
      return iter;
    }

    iterator_impl operator--(int) {
      iterator_impl iter(*this);
      --(*this);
      return iter;
    }

    iterator_impl& operator+=(int n) {
      m_x += n;
      m_y += m_x / m_img->m_width;
      m_x = m_x % m_img->m_width;
      return *this;
    }

    iterator_impl& operator-=(int n) {
      return *this += -n;
    }

    iterator_impl operator+(int n) {
      iterator_impl iter(*this);
      iter += n;
      return iter;
    }

    iterator_impl operator-(int n) const {
      iterator_impl iter(*this);
      iter -= n;
      return iter;
    }

    ptrdiff_t operator-(const iterator_impl& other) const {
      int off_self  = m_x + m_y * m_img->m_width;
      int off_other = other.m_x + other.m_y * other.m_img->m_width;
      return off_self - off_other;
    }

    IT& operator[](int n) {
      return *(*this + n);
    }

    bool operator<(const iterator_impl& other) const {
      return m_y < other.m_y || (m_y == other.m_y && m_x < other.m_x);
    }

    bool operator>(const iterator_impl& other) const {
      return m_y > other.m_y || (m_y == other.m_y && m_x > other.m_x);
    }

    bool operator<=(const iterator_impl& other) const {
      return !(*this > other);
    }

    bool operator>=(const iterator_impl& other) const {
      return !(*this < other);
    }

  private:
    friend class VectorImage<T>;
    using VectorImagePtr = typename std::conditional<std::is_const<IT>::value, const VectorImage<T>, VectorImage<T>>::type*;

    iterator_impl(VectorImagePtr img, int x, int y) : m_img(img), m_x(x), m_y(y) {}

    VectorImagePtr m_img;
    int            m_x, m_y;
  };

  using iterator       = iterator_impl<T>;
  using const_iterator = iterator_impl<const T>;

  template <typename... Args>
  static std::shared_ptr<VectorImage<T>> create(Args&&... args) {
    return std::shared_ptr<VectorImage<T>>(new VectorImage<T>(std::forward<Args>(args)...));
  }

  std::string getRepr() const final {
    return "VectorImage<" + std::to_string(m_width) + "," + std::to_string(m_height) + ">";
  }

  int getHeight() const final {
    return m_height;
  }

  int getWidth() const final {
    return m_width;
  }

  T getValue(int x, int y) const {
    return this->at(x, y);
  }

  T getValue(PixelCoordinate coord) const {
    return this->at(coord.m_x, coord.m_y);
  }

  void setValue(int x, int y, T value) final {
    at(x, y) = value;
  }

  void setValue(PixelCoordinate pc, T value) {
    setValue(pc.m_x, pc.m_y, value);
  }

  T& at(int x, int y) {
    assert(x >= 0 && y >= 0 && x < m_width && y < m_height);
    return (*m_data)[m_offset + x + y * m_stride];
  }

  const T& at(int x, int y) const {
    assert(x >= 0 && y >= 0 && x < m_width && y < m_height);
    return (*m_data)[m_offset + x + y * m_stride];
  }

  void fillValue(T value) {
    for (int y = 0; y < m_height; ++y) {
      for (int x = 0; x < m_width; ++x) {
        at(x, y) = value;
      }
    }
  }

  iterator begin() {
    return iterator(this, 0, 0);
  }

  iterator end() {
    return iterator(this, 0, m_height);
  }

  const_iterator begin() const {
    return const_iterator(this, 0, 0);
  }

  const_iterator end() const {
    return const_iterator(this, 0, m_height);
  }

  /**
   * @brief Destructor
   */
  virtual ~VectorImage() = default;

  virtual std::shared_ptr<VectorImage<T>> getChunk(int x, int y, int width, int height) const override {
    auto dst = create(width, height);
    for (int iy = 0; iy < height; ++iy) {
      for (int ix = 0; ix < width; ++ix) {
        dst->at(ix, iy) = at(ix + x, iy + y);
      }
    }
    return dst;
  }

private:
  int                             m_width, m_height;
  int                             m_offset, m_stride;
  std::shared_ptr<std::vector<T>> m_data;

}; /* End of VectorImage class */

} /* namespace SourceXtractor */

#endif
