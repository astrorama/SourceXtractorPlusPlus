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
    auto group = std::make_shared<PartialGroup>(PartialGroup {pixel_group.pixel_list, {}});
    bool safeGroup = true;

    std::set<std::shared_ptr<PartialGroup>> groups_to_connect;

    bool has_visited_left =
        m_visited_tiles.find(PixelCoordinate(m_current_tile.m_x-1, m_current_tile.m_y)) != m_visited_tiles.end();
    bool has_visited_right =
        m_visited_tiles.find(PixelCoordinate(m_current_tile.m_x+1, m_current_tile.m_y)) != m_visited_tiles.end();
    bool has_visited_top =
        m_visited_tiles.find(PixelCoordinate(m_current_tile.m_x, m_current_tile.m_y+1)) != m_visited_tiles.end();
    bool has_visited_bottom =
        m_visited_tiles.find(PixelCoordinate(m_current_tile.m_x, m_current_tile.m_y-1)) != m_visited_tiles.end();

    for (auto& pc : pixel_group.pixel_list) {
      if (pc.m_x <= m_min_x || pc.m_x >= m_max_x || pc.m_y <= m_min_y || pc.m_y >= m_max_y) {
        m_groups_on_borders[pc] = group;
        safeGroup = false;
      }

      if (pc.m_x <= m_min_x) {
        if (has_visited_left) {
          if (m_groups_on_borders.find(PixelCoordinate(pc.m_x-1, pc.m_y)) != m_groups_on_borders.end()) {
            groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x-1, pc.m_y)]);
          }
          if (m_groups_on_borders.find(PixelCoordinate(pc.m_x-1, pc.m_y-1)) != m_groups_on_borders.end()) {
            groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x-1, pc.m_y-1)]);
          }
          if (m_groups_on_borders.find(PixelCoordinate(pc.m_x-1, pc.m_y+1)) != m_groups_on_borders.end()) {
            groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x-1, pc.m_y+1)]);
          }
        } else {
          group->m_missing_tiles.emplace(PixelCoordinate(m_current_tile.m_x-1, m_current_tile.m_y));
        }
      }

      if (pc.m_x >= m_max_x) {
        if (has_visited_right) {
          if (m_groups_on_borders.find(PixelCoordinate(pc.m_x+1, pc.m_y)) != m_groups_on_borders.end()) {
            groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x+1, pc.m_y)]);
          }
          if (m_groups_on_borders.find(PixelCoordinate(pc.m_x+1, pc.m_y-1)) != m_groups_on_borders.end()) {
            groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x+1, pc.m_y-1)]);
          }
          if (m_groups_on_borders.find(PixelCoordinate(pc.m_x+1, pc.m_y+1)) != m_groups_on_borders.end()) {
            groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x+1, pc.m_y+1)]);
          }
        } else {
          group->m_missing_tiles.emplace(PixelCoordinate(m_current_tile.m_x+1, m_current_tile.m_y));
        }
      }

      if (pc.m_y <= m_min_y) {
        if (has_visited_bottom) {
          if (m_groups_on_borders.find(PixelCoordinate(pc.m_x, pc.m_y-1)) != m_groups_on_borders.end()) {
            groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x, pc.m_y-1)]);
          }
          if (m_groups_on_borders.find(PixelCoordinate(pc.m_x-1, pc.m_y-1)) != m_groups_on_borders.end()) {
            groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x-1, pc.m_y-1)]);
          }
          if (m_groups_on_borders.find(PixelCoordinate(pc.m_x+1, pc.m_y-1)) != m_groups_on_borders.end()) {
            groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x+1, pc.m_y-1)]);
          }
        } else {
          group->m_missing_tiles.emplace(PixelCoordinate(m_current_tile.m_x, m_current_tile.m_y-1));
        }
      }

      if (pc.m_y >= m_max_y) {
        if (has_visited_top) {
          if (m_groups_on_borders.find(PixelCoordinate(pc.m_x, pc.m_y+1)) != m_groups_on_borders.end()) {
            groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x, pc.m_y+1)]);
          }
          if (m_groups_on_borders.find(PixelCoordinate(pc.m_x-1, pc.m_y+1)) != m_groups_on_borders.end()) {
            groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x-1, pc.m_y+1)]);
          }
          if (m_groups_on_borders.find(PixelCoordinate(pc.m_x+1, pc.m_y+1)) != m_groups_on_borders.end()) {
            groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x+1, pc.m_y+1)]);
          }
        } else {
          group->m_missing_tiles.emplace(PixelCoordinate(m_current_tile.m_x, m_current_tile.m_y+1));
        }
      }

      // corner cases
      if (pc.m_x <= m_min_x && pc.m_y <= m_min_y) {
        if (m_visited_tiles.find(PixelCoordinate(m_current_tile.m_x-1, m_current_tile.m_y-1)) == m_visited_tiles.end()) {
          group->m_missing_tiles.emplace(PixelCoordinate(m_current_tile.m_x-1, m_current_tile.m_y-1));
        }
      }
      if (pc.m_x <= m_min_x && pc.m_y >= m_max_y) {
        if (m_visited_tiles.find(PixelCoordinate(m_current_tile.m_x-1, m_current_tile.m_y+1)) == m_visited_tiles.end()) {
          group->m_missing_tiles.emplace(PixelCoordinate(m_current_tile.m_x-1, m_current_tile.m_y+1));
        }
      }
      if (pc.m_x >= m_max_x && pc.m_y <= m_min_y) {
        if (m_visited_tiles.find(PixelCoordinate(m_current_tile.m_x+1, m_current_tile.m_y-1)) == m_visited_tiles.end()) {
          group->m_missing_tiles.emplace(PixelCoordinate(m_current_tile.m_x+1, m_current_tile.m_y-1));
        }
      }
      if (pc.m_x >= m_max_x && pc.m_y >= m_max_y) {
        if (m_visited_tiles.find(PixelCoordinate(m_current_tile.m_x+1, m_current_tile.m_y+1)) == m_visited_tiles.end()) {
          group->m_missing_tiles.emplace(PixelCoordinate(m_current_tile.m_x+1, m_current_tile.m_y+1));
        }
      }
    }

    if (safeGroup) {
      publishSource(group);
    } else {
      if (!groups_to_connect.empty()) {
        groups_to_connect.emplace(group);
        group = mergeGroups(groups_to_connect);
        group->m_missing_tiles.erase(m_current_tile);
      }
      //group->debugPrint();

      if (group->m_missing_tiles.empty()) {
        m_tile_buffer.emplace(group);
      }
    }
  }

  void flushTile() {
    //std::cout << "flush tile:\n";
    for (auto group : m_tile_buffer) {
      publishSource(group);
      for (auto& pc : group->m_pixels) {
        m_groups_on_borders.erase(pc);
      }
    }

    m_tile_buffer.clear();
  }

  void publishSource(std::shared_ptr<PartialGroup> group) {
    auto source = m_source_factory->createSource();
    source->setProperty<PixelCoordinateList>(group->m_pixels);
    source->setProperty<SourceId>();

    //std::cout << "publish id: " << source->getProperty<SourceId>().getSourceId() << " " << group->m_pixels.size() << " pixels\n";

    m_listener.publishSource(source);
  }

  std::shared_ptr<PartialGroup> mergeGroups(std::set<std::shared_ptr<PartialGroup>> groups) {
    auto merged_group = std::make_shared<PartialGroup>();

    for (auto& group : groups) {
      m_tile_buffer.erase(group);

      //std::cout << " merging ";
      //group->debugPrint();

      merged_group->m_pixels.insert(merged_group->m_pixels.end(),
          std::make_move_iterator(group->m_pixels.begin()), std::make_move_iterator(group->m_pixels.end()));

      merged_group->m_missing_tiles.insert(group->m_missing_tiles.begin(), group->m_missing_tiles.end());
    }

    for (auto& pc : merged_group->m_pixels) {
      m_groups_on_borders[pc] = merged_group;
    }

    return merged_group;
  }

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

  void flush() {
    //std::cout << "FLUSHING: \n";
    while (m_groups_on_borders.size() > 0) {
      auto group = m_groups_on_borders.begin()->second;

//      group->debugPrint();

      publishSource(group);
      for (auto& pc : group->m_pixels) {
        m_groups_on_borders.erase(pc);
      }
    }
  }

private:
  Segmentation::LabellingListener& m_listener;
  std::shared_ptr<SourceFactory> m_source_factory;

  std::unordered_map<PixelCoordinate, std::shared_ptr<PartialGroup>> m_groups_on_borders;

  int m_min_x, m_min_y;
  int m_max_x, m_max_y;

  PixelCoordinate m_current_tile;
  std::unordered_set<PixelCoordinate> m_visited_tiles;

  std::unordered_set<std::shared_ptr<PartialGroup>> m_tile_buffer;
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
    tiles_listener.flushTile();
  }
  tiles_listener.flush();
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
