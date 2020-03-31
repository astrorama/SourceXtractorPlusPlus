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
  struct PartialGroup {
    std::vector<PixelCoordinate> m_pixels;
    std::unordered_set<PixelCoordinate> m_missing_tiles;

    PartialGroup() : m_pixels(), m_missing_tiles() {}

    PartialGroup(const std::vector<PixelCoordinate>& pixels, const std::unordered_set<PixelCoordinate>& missing_tiles={})
        : m_pixels(pixels), m_missing_tiles(missing_tiles) {}

    void determineMissingTiles(PixelCoordinate current_tile, int min_x, int min_y, int max_x, int max_y) {
      bool left=false, right=false, bottom=false, top=false;
      for (auto& pc : m_pixels) {
          left   |= (pc.m_x <= min_x);
          right  |= (pc.m_x >= max_x);
          bottom |= (pc.m_y <= min_y);
          top    |= (pc.m_y >= max_y);
      }

      if (left) {
        m_missing_tiles.emplace(current_tile + PixelCoordinate(-1,0));
      }
      if (right) {
        m_missing_tiles.emplace(current_tile + PixelCoordinate(1,0));
      }
      if (bottom) {
        m_missing_tiles.emplace(current_tile + PixelCoordinate(0,-1));
      }
      if (top) {
        m_missing_tiles.emplace(current_tile + PixelCoordinate(0,1));
      }
      if (left && bottom) {
        m_missing_tiles.emplace(current_tile + PixelCoordinate(-1,-1));
      }
      if (left && top) {
        m_missing_tiles.emplace(current_tile + PixelCoordinate(-1,1));
      }
      if (right && bottom) {
        m_missing_tiles.emplace(current_tile + PixelCoordinate(1,-1));
      }
      if (right && top) {
        m_missing_tiles.emplace(current_tile + PixelCoordinate(1,1));
      }
    }

//    void debugPrint() const {
//      std::cout << "group with " << m_pixels.size() << " pixels and missing: ";
//      for (auto& pc : m_missing_tiles) {
//        std::cout << "(" << pc.m_x << ", " << pc.m_y << ") ";
//      }
//      std::cout << "\n";
//    }
  };

  TilesLabellingListener(Segmentation::LabellingListener& listener, std::shared_ptr<SourceFactory> source_factory) :
    m_listener(listener),
    m_source_factory(source_factory) {}

  virtual ~TilesLabellingListener() = default;

  void publishGroup(Lutz::PixelGroup& pixel_group) override {
    auto group = std::make_shared<PartialGroup>(pixel_group.pixel_list);

    group->determineMissingTiles(m_current_tile, m_min_x, m_min_y, m_max_x, m_max_y);

    if (group->m_missing_tiles.empty()) {
      publishSource(group);
    } else {
      for (auto& pc : group->m_pixels) {
        m_groups_on_borders[pc] = group;
      }
    }
  }

//  void evaluateTile(PixelCoordinate tile) {
//    for (auto group : m_incomplete_groups[tile]) {
//    }
////    //std::cout << "flush tile:\n";
////    for (auto group : m_tile_buffer) {
////      publishSource(group);
////      for (auto& pc : group->m_pixels) {
////        m_groups_on_borders.erase(pc);
////      }
////    }
////
////    m_tile_buffer.clear();
//  }

  void publishSource(std::shared_ptr<PartialGroup> group) {
    auto source = m_source_factory->createSource();
    source->setProperty<PixelCoordinateList>(group->m_pixels);
    source->setProperty<SourceId>();

    //std::cout << "publish id: " << source->getProperty<SourceId>().getSourceId() << " " << group->m_pixels.size() << " pixels\n";

    m_listener.publishSource(source);
  }

//  std::shared_ptr<PartialGroup> mergeGroups(std::set<std::shared_ptr<PartialGroup>> groups) {
//    auto merged_group = std::make_shared<PartialGroup>();
//
//    for (auto& group : groups) {
//      m_tile_buffer.erase(group);
//
//      //std::cout << " merging ";
//      //group->debugPrint();
//
//      merged_group->m_pixels.insert(merged_group->m_pixels.end(),
//          std::make_move_iterator(group->m_pixels.begin()), std::make_move_iterator(group->m_pixels.end()));
//
//      merged_group->m_missing_tiles.insert(group->m_missing_tiles.begin(), group->m_missing_tiles.end());
//    }
//
//    for (auto& pc : merged_group->m_pixels) {
//      m_groups_on_borders[pc] = merged_group;
//    }
//
//    return merged_group;
//  }

  void notifyProgress(int line, int total) override {
//    m_listener.notifyProgress(line, total);
//
//    if (m_window_size > 0 && line > m_window_size) {
//      m_listener.requestProcessing(ProcessSourcesEvent(LineSelectionCriteria(line - m_window_size)));
//    }
  }

  void setTileBoundary(PixelCoordinate offset, int width, int height) {
    m_min_x =  offset.m_x;
    m_max_x =  offset.m_x + width - 1;
    m_min_y =  offset.m_y;
    m_max_y =  offset.m_y + height - 1;
  }

  void setCurrentTile(PixelCoordinate pc) {
    //std::cout << "Tile " << pc.m_x << ", " << pc.m_y << "\n";
    m_current_tile = pc;
    m_visited_tiles.emplace(pc);
  }

//  void flush() {
//    //std::cout << "FLUSHING: \n";
//    while (m_groups_on_borders.size() > 0) {
//      auto group = m_groups_on_borders.begin()->second;
//
////      group->debugPrint();
//
//      publishSource(group);
//      for (auto& pc : group->m_pixels) {
//        m_groups_on_borders.erase(pc);
//      }
//    }
//  }

private:
  Segmentation::LabellingListener& m_listener;
  std::shared_ptr<SourceFactory> m_source_factory;

  std::unordered_map<PixelCoordinate, std::shared_ptr<PartialGroup>> m_groups_on_borders;

  int m_min_x, m_min_y;
  int m_max_x, m_max_y;

  PixelCoordinate m_current_tile;
  std::unordered_set<PixelCoordinate> m_visited_tiles;

  std::unordered_map<PixelCoordinate, std::shared_ptr<PartialGroup>> m_incomplete_groups;

  //std::unordered_set<std::shared_ptr<PartialGroup>> m_tile_buffer;
};

}

void TileBasedSegmentation::labelImage(Segmentation::LabellingListener& listener,
    std::shared_ptr<const DetectionImageFrame> frame) {
  Lutz lutz;
  auto image = frame->getThresholdedImage();
  TilesLabellingListener tiles_listener(listener, m_source_factory);

  int width_in_tiles = (image->getWidth() / TileManager::getInstance()->getTileWidth());
  int height_in_tiles = (image->getHeight() / TileManager::getInstance()->getTileHeight());

  for (int i=-1; i <= width_in_tiles; i++) {
    tiles_listener.setCurrentTile(PixelCoordinate(i, -1));
    tiles_listener.setCurrentTile(PixelCoordinate(i, height_in_tiles));
  }
  for (int i=-1; i <= height_in_tiles; i++) {
    tiles_listener.setCurrentTile(PixelCoordinate(-1, i));
    tiles_listener.setCurrentTile(PixelCoordinate(width_in_tiles, i));
  }

  for (auto& tile : getTiles(*image)) {
    tiles_listener.setCurrentTile(tile.coord);
    auto sub_image = SubImage<DetectionImage::PixelType>::create(image, tile.offset, tile.width, tile.height);
    tiles_listener.setTileBoundary(tile.offset, tile.width, tile.height);
    lutz.labelImage(tiles_listener, *sub_image, tile.offset);
//    tiles_listener.evaluateTile();
  }
  //tiles_listener.flush();
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
