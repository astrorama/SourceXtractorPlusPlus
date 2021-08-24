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
 * ImageChunk.h
 *
 *  Created on: Aug 30, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_IMAGECHUNK_H_
#define _SEFRAMEWORK_IMAGE_IMAGECHUNK_H_

#include <cassert>

#include "SEFramework/Image/Image.h"

namespace SourceXtractor {

template <typename T>
class ImageChunk : public Image<T> {
protected:
  template <typename Allocator>
  ImageChunk(const std::shared_ptr<std::vector<T, Allocator>> data, int offset, int width, int height, int stride)
      : m_data_holder(new AllocatorErasureImpl<Allocator>(data))
      , m_offset(offset)
      , m_stride(stride)
      , m_width(width)
      , m_height(height) {
    if (data)
      m_data_ptr = &(*data)[0];
  }

public:
  template <typename Allocator>
  static std::shared_ptr<ImageChunk<T>> create(const std::shared_ptr<std::vector<T, Allocator>> data, int offset, int width,
                                               int height, int stride) {
    return std::shared_ptr<ImageChunk<T>>(new ImageChunk<T>(data, offset, width, height, stride));
  }

  virtual ~ImageChunk() {}

  std::string getRepr() const override {
    return "ImageChunk<" + std::to_string(m_width) + "," + std::to_string(m_height) + ">";
  }

  /// Returns the value of the pixel with the coordinates (x,y)
  T getValue(int x, int y) const {
    assert(x >= 0 && y >= 0 && x < m_width && y < m_height);
    return m_data_ptr[m_offset + x + y * m_stride];
  }

  T getValue(const PixelCoordinate& coord) const {
    assert(coord.m_x >= 0 && coord.m_y >= 0 && coord.m_x < m_width && coord.m_y < m_height);
    return m_data_ptr[m_offset + coord.m_x + coord.m_y * m_stride];
  }

  /// Returns the width of the image chunk in pixels
  int getWidth() const final {
    return m_width;
  }

  /// Returns the height of the image chunk in pixels
  int getHeight() const final {
    return m_height;
  }

  virtual std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const override {
    return m_data_holder->getChunk(m_offset + x + y * m_stride, width, height, m_stride);
  }

protected:
  struct AllocatorErasure {
    virtual ~AllocatorErasure() = default;

    virtual std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const = 0;

    virtual T* get() = 0;
  };

  template <typename Allocator>
  struct AllocatorErasureImpl : public AllocatorErasure {

    AllocatorErasureImpl(std::shared_ptr<std::vector<T, Allocator>> ptr) : m_data(std::move(ptr)) {}

    virtual ~AllocatorErasureImpl() = default;

    std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const override {
      return ImageChunk<T>::create(m_data, x, y, width, height);
    }

    T* get() override {
      return m_data->data();
    }

    std::shared_ptr<std::vector<T, Allocator>> m_data;
  };

  std::unique_ptr<AllocatorErasure> m_data_holder;
  T*                                m_data_ptr;
  int                               m_offset, m_stride;
  int                               m_width, m_height;
};

template <typename T>
class UniversalImageChunk : public ImageChunk<T> {

protected:
  /**
   * This move constructor from an ImageChunk uses a dynamic cast, so if the chunk is
   * another universal chunk, we can avoid copying data, and we just move-assign the underlying vector
   */
  template <typename Allocator = std::allocator<T>>
  UniversalImageChunk(ImageChunk<T>&& chunk)
      : ImageChunk<T>(std::shared_ptr<std::vector<T, Allocator>>(nullptr), 0, chunk.getWidth(), chunk.getHeight(),
                      chunk.getWidth()) {
    UniversalImageChunk<T>* universal_ptr = dynamic_cast<UniversalImageChunk<T>*>(&chunk);
    if (universal_ptr) {
      m_data_holder = std::move(universal_ptr->m_data_holder);
      m_data_ptr    = m_data_holder->get();
    } else {
      m_data_holder.reset(new AllocatorErasureImpl<Allocator>(std::make_shared<std::vector<T>>(m_width * m_height)));
      m_data_ptr = m_data_holder->get();
      for (int cy = 0; cy < m_height; cy++) {
        for (int cx = 0; cx < m_width; cx++) {
          m_data_ptr[cx + cy * m_stride] = chunk.getValue(cx, cy);
        }
      }
    }
  }

  UniversalImageChunk(const Image<T>* image, int x, int y, int width, int height)
      : ImageChunk<T>(std::make_shared<std::vector<T>>(width * height), 0, width, height, width) {
    for (int cy = 0; cy < height; cy++) {
      for (int cx = 0; cx < width; cx++) {
        m_data_ptr[cx + cy * width] = image->getValue(x + cx, y + cy);
      }
    }
  }

  template <typename Allocator>
  UniversalImageChunk(std::vector<T, Allocator>&& data, int width, int height)
      : ImageChunk<T>(std::make_shared<std::vector<T, Allocator>>(std::move(data)), 0, width, height, width) {}

  UniversalImageChunk(int width, int height)
      : ImageChunk<T>(std::make_shared<std::vector<T>>(width * height), 0, width, height, width) {}

public:
  template <typename... Args>
  static std::shared_ptr<UniversalImageChunk<T>> create(Args&&... args) {
    return std::shared_ptr<UniversalImageChunk<T>>(new UniversalImageChunk<T>(std::forward<Args>(args)...));
  }

  virtual ~UniversalImageChunk() = default;

  void setValue(int x, int y, T value) {
    m_data_ptr[x + y * m_stride] = value;
  }

  T& at(int x, int y) {
    return m_data_ptr[x + y * m_stride];
  }

private:
  using ImageChunk<T>::m_width;
  using ImageChunk<T>::m_height;
  using ImageChunk<T>::m_stride;
  using ImageChunk<T>::m_data_ptr;
  using ImageChunk<T>::m_data_holder;

  template <typename Allocator>
  using AllocatorErasureImpl = typename ImageChunk<T>::template AllocatorErasureImpl<Allocator>;
};

}  // namespace SourceXtractor

#endif /* _SEFRAMEWORK_IMAGE_IMAGECHUNK_H_ */
