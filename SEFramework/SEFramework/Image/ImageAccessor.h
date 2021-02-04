/** Copyright © 2021 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#ifndef _SEFRAMEWORK_IMAGE_IMAGEACCESSOR_H
#define _SEFRAMEWORK_IMAGE_IMAGEACCESSOR_H

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageChunk.h"

namespace SourceXtractor {

/**
 * The Image interface used to have a getValue(x, y) method, but this was
 * suboptimal when using image types that need to keep state and be thread safe
 * (i.e. BufferedImage)
 * It was dropped, and this accessor is intended to provide an easy alternative.
 * Unlike the images, the accessor is explicitly *not thread safe* and not intended to be
 * passed around or copied.
 * To reduce performance penalties, it can be hinted about the chunk size and the
 * access pattern. For instance, if you know the access is away from the first coordinate
 * in a circular fashion, the first chunk will be centered on your first read to reduce
 * requests.
 * @tparam T
 *      Pixel type
 */
template<typename T>
class ImageAccessor: public Image<T> {
public:

  /** Hints about the access pattern
   * @details
   *    Even if you don't honor the hint, it will still work!
   */
  enum AccessHint {
    TOP_LEFT,     //< The first coordinate is likely the top left corner of what is going to be needed
    CENTERED,     //< The first coordinate is likely the center of a region
    BOTTOM_RIGHT, //< The first coordinate is likely the bottom right corner
  };

  /** Destructor */
  ~ImageAccessor() = default;

  /**
   * Constructor
   * @param img
   *    Image to be accessed
   * @param hint
   *    Hint about the access pattern
   * @param w
   *    Width of the chunk size
   * @param h
   *    Height of the chunk size
   * @note
   *    Don't be greedy over the chunk size, but not too conservative either.
   *    Of course, if you know beforehand the exact chunk that will be needed, better use
   *    getChunk directly!
   */
  ImageAccessor(std::shared_ptr<const Image<T>> img, AccessHint hint = TOP_LEFT, int w = 64, int h = 1)
    : m_image(img.get()), m_keep_alive(std::move(img)), m_hint(hint), m_read_width(w),
      m_read_height(h) {};

  ImageAccessor(const Image<T>& img, AccessHint hint = TOP_LEFT, int w = 64, int h = 64)
    : m_image(&img), m_hint(hint), m_read_width(w), m_read_height(h) {};

  /**
   * Can not be copied!
   */
  ImageAccessor(const ImageAccessor<T>&) = delete;

  /**
   * But it can be moved
   */
  ImageAccessor(ImageAccessor<T>&&) = default;

  /**
   * Can not be assigned either
   */
  ImageAccessor<T>& operator=(const ImageAccessor<T>&) = delete;

  /**
   * Drop-in replacement for Image<T>::getValue
   * @details
   *    Coordinates are always relative to the full image! As far as the caller is concerned,
   *    there is no chunk
   */
  T getValue(int x, int y) {
    selectChunk(PixelCoordinate(x, y));
    x -= m_chunk_min.m_x;
    y -= m_chunk_min.m_y;
    return m_chunk->getValue(x, y);
  }

  T getValue(const PixelCoordinate& coord) {
    selectChunk(coord);
    return m_chunk->getValue(coord - m_chunk_min);
  }

  /*
   * Forward these methods directly to the wrapped image
   */

  std::string getRepr() const override {
    return m_image->getRepr();
  }

  int getWidth() const override {
    return m_image->getWidth();
  }

  int getHeight() const override {
    return m_image->getHeight();
  }

  std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const override {
    return m_image->getChunk(x, y, width, height);
  };

private:
  const Image<T>* m_image;
  std::shared_ptr<const Image<T>> m_keep_alive;
  std::shared_ptr<const ImageChunk<T>> m_chunk;
  PixelCoordinate m_chunk_min, m_chunk_max;
  AccessHint m_hint;
  int m_read_width, m_read_height;

  /**
   * Verify if the requested coordinates can be satisfied, and asks
   * for the next chunk otherwise
   */
  void selectChunk(const PixelCoordinate& coord) {
    if (m_chunk && coord >= m_chunk_min && coord <= m_chunk_max) {
      return;
    }
    nextCoordinates(coord);
    m_chunk = m_image->getChunk(m_chunk_min, m_chunk_max);
  }

  /**
   * Try to guess what's coming next. It just uses some rudimentary heuristics.
   */
  void nextCoordinates(const PixelCoordinate& coord) {
    if (!m_chunk) {
      m_chunk_min = firstCoordinates(coord);
    }
    else {
      switch (m_hint) {
        case TOP_LEFT:
        case CENTERED:
          m_chunk_min.m_x = coord.m_x;
          m_chunk_min.m_y = coord.m_y;
          break;
        case BOTTOM_RIGHT:
          m_chunk_min.m_x = coord.m_x - m_read_width + 1;
          m_chunk_min.m_y = coord.m_y - m_read_height + 1;
          break;
      }
    }
    // Make sure we don't leave the image
    m_chunk_min.clip(m_image->getWidth(), m_image->getHeight());
    // Max pixel
    m_chunk_max.m_x = m_chunk_min.m_x + m_read_width;
    m_chunk_max.m_y = m_chunk_min.m_y + m_read_height;
    m_chunk_max.clip(m_image->getWidth(), m_image->getHeight());
  }

  PixelCoordinate firstCoordinates(const PixelCoordinate& coord) {
    switch (m_hint) {
      case CENTERED:
        return PixelCoordinate(coord.m_x - m_read_width / 2, coord.m_y - m_read_height / 2);
      case TOP_LEFT:
        return coord;
      case BOTTOM_RIGHT:
        return PixelCoordinate(coord.m_x - m_read_width, coord.m_y - m_read_height);
    }
    return coord;
  }
};

} // end of namespace SourceXtractor

#endif // _SEFRAMEWORK_IMAGE_IMAGEACCESSOR_H
