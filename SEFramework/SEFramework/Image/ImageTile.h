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
 * ImageTile.hm
 *
 *  Created on: Feb 20, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_IMAGETILE_H_
#define _SEFRAMEWORK_IMAGE_IMAGETILE_H_

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEUtils/MmapAllocator.h"
#include <iostream>

namespace SourceXtractor {

class ImageSource;

class ImageTile {
public:
  enum ImageType {
    AutoType   = -1,
    FloatImage = 0,
    DoubleImage,
    IntImage,
    UIntImage,
    LongLongImage,
  };

  static std::shared_ptr<ImageTile> create(ImageType image_type, int x, int y, int width, int height,
                                           std::shared_ptr<ImageSource> source = nullptr);

  virtual ~ImageTile() = default;

  bool isPixelInTile(int x, int y) const {
    return x >= m_x && y >= m_y && x < m_max_x && y < m_max_y;
  }

  int getPosX() const {
    return m_x;
  }

  int getPosY() const {
    return m_y;
  }

  virtual int getTileMemorySize() const = 0;

  int getWidth() const {
    return m_max_x - m_x;
  }

  int getHeight() const {
    return m_max_y - m_y;
  }

  template <typename T>
  T getValue(int x, int y) const {
    T value;
    getValue(x, y, value);
    return value;
  }

  virtual void setValue(int x, int y, float value)        = 0;
  virtual void setValue(int x, int y, double value)       = 0;
  virtual void setValue(int x, int y, int value)          = 0;
  virtual void setValue(int x, int y, unsigned int value) = 0;
  virtual void setValue(int x, int y, std::int64_t value) = 0;

  virtual void* getDataPtr() = 0;

  void setModified(bool modified) {
    m_modified = modified;
  }

  bool isModified() const {
    return m_modified;
  }

  void saveIfModified();

  static ImageType getTypeValue(float) {
    return FloatImage;
  }

  static ImageType getTypeValue(double) {
    return DoubleImage;
  }

  static ImageType getTypeValue(int) {
    return IntImage;
  }

  static ImageType getTypeValue(unsigned int) {
    return UIntImage;
  }

  static ImageType getTypeValue(std::int64_t) {
    return LongLongImage;
  }

  static size_t getTypeSize(ImageType image_type) {
    switch (image_type) {
    default:
    case ImageTile::FloatImage:
    case ImageTile::IntImage:
    case ImageTile::UIntImage:
      return 4;
    case ImageTile::LongLongImage:
    case ImageTile::DoubleImage:
      return 8;
    }
  }

  ImageType getType() const {
    return m_image_type;
  }

protected:
  virtual void getValue(int x, int y, float& value) const        = 0;
  virtual void getValue(int x, int y, double& value) const       = 0;
  virtual void getValue(int x, int y, int& value) const          = 0;
  virtual void getValue(int x, int y, unsigned int& value) const = 0;
  virtual void getValue(int x, int y, std::int64_t& value) const = 0;

  ImageTile(ImageType image_type, int x, int y, int width, int height, std::shared_ptr<ImageSource> source = nullptr)
      : m_modified(false), m_image_type(image_type), m_source(source), m_x(x), m_y(y), m_max_x(x + width), m_max_y(y + height) {}

  ImageTile(const ImageTile&) = delete;
  ImageTile(ImageTile&&)      = delete;
  ImageTile& operator=(const ImageTile&) = delete;
  ImageTile& operator=(ImageTile&&) = delete;

  bool                         m_modified;
  ImageType                    m_image_type;
  std::shared_ptr<ImageSource> m_source;
  int                          m_x, m_y;
  int                          m_max_x, m_max_y;
};

/**
 * ImageTile with an associated type
 * @tparam T
 * @tparam Allocator
 * @details
 *  The memory is allocated via mmap instead of std::allocator. The reason is that
 *  tiles are allocated and deallocated continuously from different threads, and this may
 *  cause an increased memory consumption when using glibc malloc.
 *
 *  For instance, thread 1 may be allocating tiles while segmenting. When thread 2 has a cache miss,
 *  it will de-allocate a tile, and allocate a new one. However with glibc's malloc, the de-allocation
 *  will happen on thread arena, and the allocation on thread 2 arena. If then thread 3 does the same,
 *  now two tiles were freed from the main arena but never given back to the system *unless* they were on the top
 *  of the heap (unlikely, probably something else is living there by then), while two new tiles allocated
 *  of two different arenas. We have no leak, but the equivalent of 4 tiles have been taken from the RAM.
 *
 *  With enough threads and tiles big enough, this can pile up quickly.
 */
template <typename T, typename Allocator = MmapAllocator<T>>
class ImageTileWithType final : public ImageTile {
public:
  ImageTileWithType(int x, int y, int width, int height, std::shared_ptr<ImageSource> source)
      : ImageTile(getTypeValue(T()), x, y, width, height, source) {
    m_tile_image = VectorImage<T, Allocator>::create(width, height);
  }

  virtual ~ImageTileWithType() {
    saveIfModified();
  }

  int getTileMemorySize() const override {
    return getWidth() * getHeight() * sizeof(T);
  }

  void* getDataPtr() override {
    return m_tile_image->begin();
  }

  const std::shared_ptr<VectorImage<T, Allocator>>& getImage() const {
    return m_tile_image;
  }

  template <typename U>
  void getValueImpl(int x, int y, U& value) const {
    assert(isPixelInTile(x, y));
    value = m_tile_image->getValue(x - m_x, y - m_y);
  }

  template <typename U>
  void setValueImpl(int x, int y, U value) {
    assert(isPixelInTile(x, y));
    m_tile_image->setValue(x - m_x, y - m_y, value);
  }

  void getValue(int x, int y, float& value) const override {
    getValueImpl(x, y, value);
  }

  void getValue(int x, int y, double& value) const override {
    getValueImpl(x, y, value);
  }

  void getValue(int x, int y, int& value) const override {
    getValueImpl(x, y, value);
  }

  void getValue(int x, int y, unsigned int& value) const override {
    getValueImpl(x, y, value);
  }

  void getValue(int x, int y, std::int64_t& value) const override {
    getValueImpl(x, y, value);
  }

  void setValue(int x, int y, float value) override {
    setValueImpl(x, y, value);
  }

  void setValue(int x, int y, double value) override {
    setValueImpl(x, y, value);
  }

  void setValue(int x, int y, int value) override {
    setValueImpl(x, y, value);
  }

  void setValue(int x, int y, unsigned int value) override {
    setValueImpl(x, y, value);
  }

  void setValue(int x, int y, std::int64_t value) override {
    setValueImpl(x, y, value);
  }

private:
  std::shared_ptr<VectorImage<T, Allocator>> m_tile_image;
};

}  // namespace SourceXtractor

#endif /* _SEFRAMEWORK_IMAGE_IMAGETILE_H_ */
