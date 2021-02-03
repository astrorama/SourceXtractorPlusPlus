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

#include "SEFramework/Image/BufferedImage.h"
#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/Image/ImageTile.h"
#include "SEFramework/Image/TileManager.h"

namespace SourceXtractor {

template<typename T>
BufferedImage<T>::BufferedImage(std::shared_ptr<const ImageSource> source,
                                std::shared_ptr<TileManager> tile_manager)
  : m_source(source), m_tile_manager(tile_manager) {}


template<typename T>
std::shared_ptr<BufferedImage<T>> BufferedImage<T>::create(std::shared_ptr<const ImageSource> source,
                                                           std::shared_ptr<TileManager> tile_manager) {
  return std::shared_ptr<BufferedImage<T>>(new BufferedImage<T>(source, tile_manager));
}


template<typename T>
std::string BufferedImage<T>::getRepr() const {
  return "BufferedImage(" + m_source->getRepr() + ")";
}


template<typename T>
int BufferedImage<T>::getWidth() const {
  return m_source->getWidth();
}


template<typename T>
int BufferedImage<T>::getHeight() const {
  return m_source->getHeight();
}


template<typename T>
std::shared_ptr<ImageChunk<T>> BufferedImage<T>::getChunk(int x, int y, int width, int height) const {
  int tile_width = m_tile_manager->getTileWidth();
  int tile_height = m_tile_manager->getTileHeight();
  int tile_offset_x = x % tile_width;
  int tile_offset_y = y % tile_height;

  // When the chunk does *not* cross boundaries, we can just use the memory hold by the single tile
  if (tile_offset_x + width <= tile_width && tile_offset_y + height <= tile_height) {
    // the tile image is going to be kept in memory as long as the chunk exists, but it could be unloaded
    // from TileManager and even reloaded again, wasting memory,
    // however image chunks are normally short lived so it's probably OK
    auto tile = m_tile_manager->getTileForPixel(x, y, m_source);
    // The tile may be smaller than tile_width x tile_height if the image is smaller, or does not divide neatly!

    auto image = tile->getImage<T>();
    return image->getChunk(tile_offset_x, tile_offset_y, width, height);
  }
  else {
    // If the chunk cross boundaries, we can't just use the memory from within a tile, so we need to copy
    // To avoid the overhead of calling getValue() - which uses a thread local - we do the full thing here
    // Also, instead of iterating on the pixel coordinates, to avoid asking several times for the same tile,
    // iterate over the tiles
    std::vector<T> data(width * height);
    int tile_w = m_tile_manager->getTileWidth();
    int tile_h = m_tile_manager->getTileHeight();

    int tile_start_x = x / tile_w * tile_w;
    int tile_start_y = y / tile_h * tile_h;
    int tile_end_x = (x + width - 1) / tile_w * tile_w;
    int tile_end_y = (y + height - 1) / tile_h * tile_h;

    for (int iy = tile_start_y; iy <= tile_end_y; iy += tile_h) {
      for (int ix = tile_start_x; ix <= tile_end_x; ix += tile_w) {
        auto tile = m_tile_manager->getTileForPixel(ix, iy, m_source);
        copyOverlappingPixels(*tile, data, x, y, width, height, tile_w, tile_h);
      }
    }

    return UniversalImageChunk<T>::create(std::move(data), width, height);
  }
}


template<typename T>
void BufferedImage<T>::copyOverlappingPixels(const ImageTile& tile, std::vector<T>& output,
                                             int x, int y, int w, int h,
                                             int tile_w, int tile_h) const {
  int start_x = std::max(tile.getPosX(), x);
  int start_y = std::max(tile.getPosY(), y);
  int end_x = std::min(tile.getPosX() + tile_w, x + w);
  int end_y = std::min(tile.getPosY() + tile_h, y + h);
  int off_x = start_x - x;
  int off_y = start_y - y;

  for (int data_y = off_y, img_y = start_y; img_y < end_y; ++data_y, ++img_y) {
    for (int data_x = off_x, img_x = start_x; img_x < end_x; ++data_x, ++img_x) {
      output[data_x + data_y * w] = tile.getValue<T>(img_x, img_y);
    }
  }
}


template class BufferedImage<MeasurementImage::PixelType>;
template class BufferedImage<FlagImage::PixelType>;
template class BufferedImage<unsigned int>;
template class BufferedImage<int>;
template class BufferedImage<double>;


} // end namespace SourceXtractor
