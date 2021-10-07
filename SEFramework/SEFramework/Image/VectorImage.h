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
  VectorImage(const VectorImage<T>& other);

  VectorImage(VectorImage<T>&& other) = default;

  VectorImage(int width, int height);

  VectorImage(int width, int height, std::vector<T>&& data);

  VectorImage(int width, int height, const std::vector<T>& data);

  template <typename Iter>
  VectorImage(int width, int height, Iter data_begin, Iter data_end,
              typename std::enable_if<
                  std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<Iter>::iterator_category>::value and
                  std::is_same<T, typename std::iterator_traits<Iter>::value_type>::value>::type* = 0);

  VectorImage(const Image<T>& other);

  VectorImage(const std::shared_ptr<const Image<T>>& other);

  VectorImage(int width, int height, std::shared_ptr<std::vector<T>> data, int offset, int stride);

public:
  template <typename IT>
  class iterator_impl : public std::iterator<std::random_access_iterator_tag, IT> {
  public:
    iterator_impl(const iterator_impl&) = default;
    iterator_impl(iterator_impl&&)      = default;

    iterator_impl& operator=(const iterator_impl&) = default;

    IT& operator*();

    bool operator==(const iterator_impl& other) const;

    bool operator!=(const iterator_impl& other) const;

    iterator_impl& operator++();

    iterator_impl& operator--();

    iterator_impl operator++(int);

    iterator_impl operator--(int);

    iterator_impl& operator+=(int n);

    iterator_impl& operator-=(int n);

    iterator_impl operator+(int n);

    iterator_impl operator-(int n) const;

    ptrdiff_t operator-(const iterator_impl& other) const;

    IT& operator[](int n);

    bool operator<(const iterator_impl& other) const;

    bool operator>(const iterator_impl& other) const;

    bool operator<=(const iterator_impl& other) const;

    bool operator>=(const iterator_impl& other) const;

  private:
    friend class VectorImage<T>;
    using VectorImagePtr = typename std::conditional<std::is_const<IT>::value, const VectorImage<T>, VectorImage<T>>::type*;

    iterator_impl(VectorImagePtr img, int x, int y);

    VectorImagePtr m_img;
    int            m_x, m_y;
  };

  using iterator       = iterator_impl<T>;
  using const_iterator = iterator_impl<const T>;

  template <typename... Args>
  static std::shared_ptr<VectorImage<T>> create(Args&&... args);

  std::string getRepr() const final;

  int getHeight() const final;

  int getWidth() const final;

  T getValue(int x, int y) const;

  T getValue(PixelCoordinate coord) const;

  void setValue(int x, int y, T value) final;

  void setValue(PixelCoordinate pc, T value);

  T& at(int x, int y);

  const T& at(int x, int y) const;

  void fillValue(T value);

  iterator begin();

  iterator end();

  const_iterator begin() const;

  const_iterator end() const;

  /**
   * @brief Destructor
   */
  virtual ~VectorImage() = default;

  std::shared_ptr<VectorImage<T>> getChunk(int x, int y, int width, int height) const final;

private:
  int                             m_width, m_height;
  int                             m_offset, m_stride;
  std::shared_ptr<std::vector<T>> m_data;

}; /* End of VectorImage class */

} /* namespace SourceXtractor */

#include "SEFramework/Image/_impl/VectorImage.icpp"

#endif
