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
 * InterpolatedImageSource.h
 *
 *  Created on: Jun 21, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef SEFRAMEWORK_SEFRAMEWORK_IMAGE_INTERPOLATEDIMAGESOURCE_H_
#define SEFRAMEWORK_SEFRAMEWORK_IMAGE_INTERPOLATEDIMAGESOURCE_H_

#include "SEFramework/Image/ProcessingImageSource.h"

namespace SourceXtractor {

/**
 * Interpolates an image using a variance map: substitute "bad" pixels with the nearest "good" one
 */
template<typename T>
class InterpolatedImageSource : public ProcessingImageSource<T> {
public:
  InterpolatedImageSource(std::shared_ptr<Image<T>> image, std::shared_ptr<WeightImage> variance_map,
                          WeightImage::PixelType variance_threshold, int interpolation_gap)
    : ProcessingImageSource<T>(image),
      m_variance_map(variance_map), m_variance_threshold(variance_threshold),
      m_interpolation_gap(interpolation_gap) {
  }

  std::string getRepr() const override {
    return "InterpolatedImageSource(" + getImageRepr() + "," + m_variance_map->getRepr() + ")";
  }

protected:
  using ProcessingImageSource<T>::getImageRepr;

  void generateTile(const std::shared_ptr<Image<T>>& image, ImageTile<T>& tile,
                    int x, int y, int width, int height) const override {
    // Get the chunks we are interested in, and its surrounding area so we can convolve
    auto chunk_start_x = x - m_interpolation_gap;
    auto chunk_end_x = x + width;
    auto chunk_start_y = y - m_interpolation_gap;
    auto chunk_end_y = y + height;

    // Remember to clip to avoid accessing the image out of bounds!
    auto chunk_pixel_x = std::max(chunk_start_x, 0);
    auto chunk_pixel_y = std::max(chunk_start_y, 0);
    auto chunk_w = std::min(chunk_end_x - chunk_pixel_x, image->getWidth() - chunk_pixel_x);
    auto chunk_h = std::min(chunk_end_y - chunk_pixel_y, image->getHeight() - chunk_pixel_y);

    // Get the chunks for the image and the variance
    // We rely on the underlying chain to generate them efficiently (i.e. BufferedImage across tiles)
    auto img_chunk = image->getChunk(chunk_pixel_x, chunk_pixel_y, chunk_w, chunk_h);
    auto variance_chunk = m_variance_map->getChunk(chunk_pixel_x, chunk_pixel_y, chunk_w, chunk_h);

    // Fill the tile interpolating from the chunks
    int off_x = x - chunk_pixel_x;
    int off_y = y - chunk_pixel_y;
    auto& tile_data = *tile.getImage();
    for (int iy = 0; iy < height; ++iy) {
      for (int ix = 0; ix < width; ++ix) {
        tile_data.at(ix, iy) = getInterpolatedValue(*img_chunk, *variance_chunk, ix + off_x, iy + off_y);
      }
    }
  }

private:
  std::shared_ptr<WeightImage> m_variance_map;
  WeightImage::PixelType m_variance_threshold;
  int m_interpolation_gap;

  inline T getInterpolatedValue(const ImageChunk<T>& img, const ImageChunk<T>& var, int x, int y) const {
    if (var.getValue(x, y) < m_variance_threshold)
      return img.getValue(x, y);

    for (int i = 1; i <= m_interpolation_gap; i++) {
      if (x - i >= 0 && var.getValue(x - i, y) < m_variance_threshold) {
        return img.getValue(x - i, y);
      }
      if (y - i >= 0 && var.getValue(x, y - i) < m_variance_threshold) {
        return img.getValue(x, y - i);
      }
    }
    return img.getValue(x, y);
  }
};

} // end namespace SourceXtractor

#endif
