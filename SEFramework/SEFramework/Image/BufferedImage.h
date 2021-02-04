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
 * BufferedImage.h
 *
 *  Created on: Feb 14, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_BUFFEREDIMAGE_H_
#define _SEFRAMEWORK_IMAGE_BUFFEREDIMAGE_H_

#include <mutex>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/TileManager.h"


namespace SourceXtractor {

/**
 *
 */
template <typename T>
class BufferedImage : public virtual Image<T> {
protected:

  BufferedImage(std::shared_ptr<const ImageSource> source, std::shared_ptr<TileManager> tile_manager);

public:
  virtual ~BufferedImage() = default;

  static std::shared_ptr<BufferedImage<T>> create(std::shared_ptr<const ImageSource> source,
      std::shared_ptr<TileManager> tile_manager = TileManager::getInstance());

  std::string getRepr() const override;

  /// Returns the width of the image in pixels
  int getWidth() const override;

  /// Returns the height of the image in pixels
  int getHeight() const override;

  std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const override;

protected:
  std::shared_ptr<const ImageSource> m_source;
  std::shared_ptr<TileManager> m_tile_manager;

  void copyOverlappingPixels(const ImageTile &tile, std::vector<T> &output,
                             int x, int y, int w, int h,
                             int tile_w, int tile_h) const;
};

}


#endif /* _SEFRAMEWORK_IMAGE_BUFFEREDIMAGE_H_ */
