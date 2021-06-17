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

template<typename T>
class ImageChunk : public Image<T> {
protected:
  ImageChunk(std::shared_ptr<const std::vector<T>> data, int offset, int width, int height,
             int stride)
    : m_data(data),
      m_offset(offset), m_stride(stride),
      m_width(width), m_height(height) {}

public:
  static std::shared_ptr<ImageChunk<T>>
  create(std::shared_ptr<const std::vector<T>> data, int offset, int width, int height, int stride) {
    return std::shared_ptr<ImageChunk<T>>(new ImageChunk<T>(data, offset, width, height, stride));
  }

  virtual ~ImageChunk() {
  }

  std::string getRepr() const override {
    return "ImageChunk<" + std::to_string(m_width) + "," + std::to_string(m_height) + ">";
  }

  /// Returns the value of the pixel with the coordinates (x,y)
  T getValue(int x, int y) const {
    assert(x >= 0 && y >=0 && x < m_width && y < m_height);
    return (*m_data)[m_offset + x + y * m_stride];
  }

  T getValue(const PixelCoordinate& coord) const {
    assert(coord.m_x >= 0 && coord.m_y >=0 && coord.m_x < m_width && coord.m_y < m_height);
    return (*m_data)[m_offset + coord.m_x + coord.m_y * m_stride];
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
    return create(m_data, m_offset + x + y * m_stride, width, height, m_stride);
  }

protected:
  std::shared_ptr<const std::vector<T>> m_data;
  int m_offset, m_stride;
  int m_width, m_height;
};

template <typename T>
class UniversalImageChunk : public ImageChunk<T> {

protected:

  /**
   * This move constructor from an ImageChunk uses a dynamic cast, so if the chunk is
   * another universal chunk, we can avoid copying data, and we just move-assign the underlying vector
   */
  UniversalImageChunk(ImageChunk<T>&& chunk) :
    ImageChunk<T>(nullptr, 0, chunk.getWidth(), chunk.getHeight(), chunk.getWidth()) {
    UniversalImageChunk<T>* universal_ptr = dynamic_cast<UniversalImageChunk<T>*>(&chunk);
    if (universal_ptr) {
      m_chunk_vector = std::move(universal_ptr->m_chunk_vector);
    }
    else {
      m_chunk_vector  = std::make_shared<std::vector<T>>(m_width * m_height);
      for (int cy = 0; cy < m_height; cy++) {
        for (int cx = 0; cx < m_width; cx++) {
          (*m_chunk_vector)[cx + cy * m_stride] = chunk.getValue(cx, cy);
        }
      }
    }
    m_data = m_chunk_vector;
  }

  UniversalImageChunk(const Image <T> *image, int x, int y, int width, int height)
    : ImageChunk<T>(nullptr, 0, width, height, width),
      m_chunk_vector(std::make_shared<std::vector<T>>(width * height)) {

    m_data = m_chunk_vector;

    for (int cy = 0; cy < height; cy++) {
      for (int cx = 0; cx < width; cx++) {
        (*m_chunk_vector)[cx + cy * width] = image->getValue(x + cx, y + cy);
      }
    }
  }

  UniversalImageChunk(std::vector<T> &&data, int width, int height):
    ImageChunk<T>(nullptr, 0, width, height, width),
    m_chunk_vector(std::make_shared<std::vector<T>>(std::move(data)))
  {
    assert(static_cast<int>(m_chunk_vector->size()) == width * height);
    m_data = m_chunk_vector;
  }

  UniversalImageChunk(int width, int height) :
    ImageChunk<T>(nullptr, 0, width, height, width),
    m_chunk_vector(std::make_shared<std::vector<T>>(width * height)) {
    assert(static_cast<int>(m_chunk_vector->size()) == width * height);
    m_data = m_chunk_vector;
  }

public:
  template <typename... Args>
  static std::shared_ptr<UniversalImageChunk<T>> create(Args&&... args) {
    return std::shared_ptr<UniversalImageChunk<T>>(new UniversalImageChunk<T>(std::forward<Args>(args)...));
  }

  virtual ~UniversalImageChunk() {
  }

  void setValue(int x, int y, T value) {
    (*m_chunk_vector)[x + y * m_stride] = value;
  }

  T& at(int x, int y) {
    return (*m_chunk_vector)[x + y * m_stride];
  }

private:
  std::shared_ptr<std::vector<T>> m_chunk_vector;
  using ImageChunk<T>::m_width;
  using ImageChunk<T>::m_height;
  using ImageChunk<T>::m_stride;
  using ImageChunk<T>::m_data;
};


}

#endif /* _SEFRAMEWORK_IMAGE_IMAGECHUNK_H_ */
