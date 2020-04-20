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

#include "SEUtils/HilbertCurve.h"

#include "SEFramework/Image/SubImage.h"
#include "SEFramework/Image/TileManager.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Segmentation/Lutz.h"
#include "SEImplementation/Property/SourceId.h"

#include "SEImplementation/Segmentation/TileBasedSegmentation.h"

namespace SourceXtractor {

namespace {

class TilesLabellingListener : public Lutz::LutzListener {
public:

  enum {
    MISSING_TOP_TILE = 1,
    MISSING_TOP_RIGHT_TILE = 2,
    MISSING_RIGHT_TILE = 4,
    MISSING_BOTTOM_RIGHT_TILE = 8,
    MISSING_BOTTOM_TILE = 16,
    MISSING_BOTTOM_LEFT_TILE = 32,
    MISSING_LEFT_TILE = 64,
    MISSING_TOP_LEFT_TILE = 128,
  };


  struct PartialGroup {
    std::vector<PixelCoordinate> m_pixels;
    PixelCoordinate m_tile_coordinate;
    unsigned int m_missing_tiles;

    PartialGroup() : m_pixels(), m_tile_coordinate(), m_missing_tiles(0) {}

    PartialGroup(const std::vector<PixelCoordinate>& pixels, PixelCoordinate tile_coordinate)
        : m_pixels(pixels), m_tile_coordinate(tile_coordinate), m_missing_tiles(0) {}
  };

  TilesLabellingListener(Segmentation::LabellingListener& listener,
      std::shared_ptr<SourceFactory> source_factory, int tile_width, int tile_height, int width, int height) :
    m_listener(listener),
    m_source_factory(source_factory),
    m_width(width), m_height(height),
    m_tile_width(tile_width), m_tile_height(tile_height) {}

  virtual ~TilesLabellingListener() = default;

  void startTileSegmentation(PixelCoordinate tile_coordinate) {
    m_current_tile = tile_coordinate;
  }

  // Called by the Lutz algorithm for each detection on a tile
  void publishGroup(Lutz::PixelGroup& pixel_group) override {
    auto group = std::make_shared<PartialGroup>(pixel_group.pixel_list, m_current_tile);

    processGroup(group);
    if (group->m_missing_tiles == 0) {
      publishSource(group);
    } else {
    }
  }

  void processGroup(std::shared_ptr<PartialGroup> group) {
    for (auto& pc : group->m_pixels) {
      unsigned int pixel_mask = 0;
      if (pc.m_x == 0) {
        pixel_mask |= MISSING_LEFT_TILE;
      }
      if (pc.m_y == 0) {
        pixel_mask |= MISSING_BOTTOM_TILE;
      }
      if (pc.m_x == m_tile_width-1) {
        pixel_mask |= MISSING_RIGHT_TILE;
      }
      if (pc.m_y == m_tile_height-1) {
        pixel_mask |= MISSING_TOP_TILE;
      }
      group->m_missing_tiles |= pixel_mask;

      if (pixel_mask != 0) {

      }
    }
  }

  void endTileSegmentation() {
    m_visited_tiles.emplace(m_current_tile);
  }

  void publishSource(std::shared_ptr<PartialGroup> group) {
    std::vector<PixelCoordinate> pixels;
    PixelCoordinate offset(group->m_tile_coordinate.m_x *  m_tile_width, group->m_tile_coordinate.m_y *  m_tile_height);
    for (auto& pc : group->m_pixels) {
      pixels.emplace_back(pc + offset);
    }

    auto source = m_source_factory->createSource();
    source->setProperty<PixelCoordinateList>(pixels);
    source->setProperty<SourceId>();

    m_listener.publishSource(source);
  }

  void notifyProgress(int line, int total) override {
    //m_listener.notifyProgress(line, total);
  }


private:
  Segmentation::LabellingListener& m_listener;
  std::shared_ptr<SourceFactory> m_source_factory;
  int m_width, m_height;
  int m_tile_width, m_tile_height;


  PixelCoordinate m_current_tile;
  std::unordered_set<PixelCoordinate> m_visited_tiles;
};

}

//
// TileBasedSegmentation
//

void TileBasedSegmentation::labelImage(Segmentation::LabellingListener& listener,
    std::shared_ptr<const DetectionImageFrame> frame) {
  Lutz lutz;
  auto image = frame->getThresholdedImage();

  // Mark all tiles surrounding the image as "visited" since they will never be processed and we shouldn't wait for them
  int width_in_tiles = (image->getWidth() / TileManager::getInstance()->getTileWidth());
  int height_in_tiles = (image->getHeight() / TileManager::getInstance()->getTileHeight());

  TilesLabellingListener tiles_listener(listener, m_source_factory,
      TileManager::getInstance()->getTileWidth(), TileManager::getInstance()->getTileHeight(),
      width_in_tiles, height_in_tiles);


  // Process image tiles in the Hilbert curve order
  for (auto& tile : getTiles(*image)) {
    auto sub_image = SubImage<DetectionImage::PixelType>::create(image, tile.offset, tile.width, tile.height);

    tiles_listener.startTileSegmentation(tile.coord);
    lutz.labelImage(tiles_listener, *sub_image);
    tiles_listener.endTileSegmentation();
  }
}

std::vector<TileBasedSegmentation::Tile> TileBasedSegmentation::getTiles(const DetectionImage& image) const {
  int tile_width = TileManager::getInstance()->getTileWidth();
  int tile_height = TileManager::getInstance()->getTileHeight();

  std::vector<TileBasedSegmentation::Tile> tiles;


  int size = std::max((image.getWidth() + tile_width - 1) / tile_width,
                      (image.getHeight() + tile_height - 1) / tile_height);

  HilbertCurve curve(size);
  for (auto& coord : curve.getCurve()) {
    int x = coord.m_x * tile_width;
    int y = coord.m_y * tile_height;

    if (x < image.getWidth() && y < image.getHeight()) {
      tiles.emplace_back(TileBasedSegmentation::Tile {
        coord,
        PixelCoordinate(x, y),
        std::min(tile_width, image.getWidth() - x),
        std::min(tile_height, image.getHeight() - y)
      });
    }
  }

  return tiles;
}

}
