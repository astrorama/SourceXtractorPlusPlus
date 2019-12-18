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

#include <algorithm>

#include "SEUtils/Misc.h"

#include "SEFramework/Image/SubImage.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Segmentation/Lutz.h"
#include "SEImplementation/Property/SourceId.h"

#include "SEImplementation/Segmentation/TileBasedSegmentation.h"

namespace SourceXtractor {

namespace {

class TilesLabellingListener : public Lutz::LutzListener {
public:
  TilesLabellingListener(Segmentation::LabellingListener& listener, std::shared_ptr<SourceFactory> source_factory) :
    m_listener(listener),
    m_source_factory(source_factory) {}

  virtual ~TilesLabellingListener() = default;

  void publishGroup(Lutz::PixelGroup& pixel_group) override {
    auto source = m_source_factory->createSource();
    source->setProperty<PixelCoordinateList>(pixel_group.pixel_list);
    source->setProperty<SourceId>();
    m_listener.publishSource(source);
  }

  void notifyProgress(int line, int total) override {
//    m_listener.notifyProgress(line, total);
//
//    if (m_window_size > 0 && line > m_window_size) {
//      m_listener.requestProcessing(ProcessSourcesEvent(LineSelectionCriteria(line - m_window_size)));
//    }
  }

private:
  Segmentation::LabellingListener& m_listener;
  std::shared_ptr<SourceFactory> m_source_factory;
};

}

void TileBasedSegmentation::labelImage(Segmentation::LabellingListener& listener,
    std::shared_ptr<const DetectionImageFrame> frame) {
  Lutz lutz;
  TilesLabellingListener tiles_listener(listener, m_source_factory);

  auto image = frame->getThresholdedImage();
  for (auto& tile : getTiles(*image)) {
    auto sub_image = SubImage<DetectionImage::PixelType>::create(image, tile.offset, tile.width, tile.height);
    lutz.labelImage(tiles_listener, *sub_image, tile.offset);
  }
}

std::vector<TileBasedSegmentation::Tile> TileBasedSegmentation::getTiles(const DetectionImage& image) const {
  int tile_width = TileManager::getInstance()->getTileWidth();
  int tile_height = TileManager::getInstance()->getTileHeight();

  std::vector<TileBasedSegmentation::Tile> tiles;


  int size = std::max((image.getWidth() + tile_width - 1) / tile_width,
                      (image.getHeight() + tile_height - 1) / tile_height);

  for (auto& coord : getHilbertCurve(size)) {
    int x = coord.m_x * tile_width;
    int y = coord.m_y * tile_height;

    if (x < image.getWidth() && y < image.getHeight()) {
      tiles.emplace_back(TileBasedSegmentation::Tile {
        PixelCoordinate(x, y),
        std::min(tile_width, image.getWidth() - x),
        std::min(tile_height, image.getHeight() - y)
      });
    }
  }

  return tiles;
}

std::vector<PixelCoordinate> TileBasedSegmentation::getHilbertCurve(unsigned int size) const {
  size = nextPowerOfTwo(size);

  std::vector<PixelCoordinate> hilbert_curve;
  for (unsigned int i = 0; i < size * size; i++) {
    hilbert_curve.emplace_back(getHilbertCurveCoordinate(i, size));
  }
  return hilbert_curve;
}

PixelCoordinate TileBasedSegmentation::getHilbertCurveCoordinate(unsigned int index, unsigned int n) const {
  unsigned int x = 0;
  unsigned int y = 0;
  for (unsigned int s=1; s<n; s*=2) {
    unsigned int rx = 1 & (index >> 1);
    unsigned int ry = 1 & (index ^ rx);

    if (ry == 0) {
      if (rx == 1) {
        x = s-1 - x;
        y = s-1 - y;
      }

      std::swap(x, y);
    }

    x += s * rx;
    y += s * ry;
    index >>= 2;
  }

  return {(int) x, (int) y};
}

}
