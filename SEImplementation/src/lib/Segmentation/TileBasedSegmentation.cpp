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
    bool process_now = true;
    auto group = std::make_shared<std::vector<PixelCoordinate>>(pixel_group.pixel_list);

    std::set<std::shared_ptr<std::vector<PixelCoordinate>>> groups_to_connect;
    for (auto& pc : pixel_group.pixel_list) {
      if (pc.m_x <= m_min_x || pc.m_x >= m_max_x || pc.m_y <= m_min_y || pc.m_y >= m_max_y) {
        process_now = false;
        m_groups_on_borders[pc] = group;
      }

      if (pc.m_x <= m_min_x) {
        if (m_groups_on_borders.find(PixelCoordinate(pc.m_x-1, pc.m_y)) != m_groups_on_borders.end()) {
          groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x-1, pc.m_y)]);
        }
        if (m_groups_on_borders.find(PixelCoordinate(pc.m_x-1, pc.m_y-1)) != m_groups_on_borders.end()) {
          groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x-1, pc.m_y-1)]);
        }
        if (m_groups_on_borders.find(PixelCoordinate(pc.m_x-1, pc.m_y+1)) != m_groups_on_borders.end()) {
          groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x-1, pc.m_y+1)]);
        }
      }
      if (pc.m_x >= m_max_x) {
        if (m_groups_on_borders.find(PixelCoordinate(pc.m_x+1, pc.m_y)) != m_groups_on_borders.end()) {
          groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x+1, pc.m_y)]);
        }
        if (m_groups_on_borders.find(PixelCoordinate(pc.m_x+1, pc.m_y-1)) != m_groups_on_borders.end()) {
          groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x+1, pc.m_y-1)]);
        }
        if (m_groups_on_borders.find(PixelCoordinate(pc.m_x+1, pc.m_y+1)) != m_groups_on_borders.end()) {
          groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x+1, pc.m_y+1)]);
        }
      }
      if (pc.m_y <= m_min_y) {
        if (m_groups_on_borders.find(PixelCoordinate(pc.m_x, pc.m_y-1)) != m_groups_on_borders.end()) {
          groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x, pc.m_y-1)]);
        }
        if (m_groups_on_borders.find(PixelCoordinate(pc.m_x-1, pc.m_y-1)) != m_groups_on_borders.end()) {
          groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x-1, pc.m_y-1)]);
        }
        if (m_groups_on_borders.find(PixelCoordinate(pc.m_x+1, pc.m_y-1)) != m_groups_on_borders.end()) {
          groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x+1, pc.m_y-1)]);
        }
      }
      if (pc.m_y >= m_max_y) {
        if (m_groups_on_borders.find(PixelCoordinate(pc.m_x, pc.m_y+1)) != m_groups_on_borders.end()) {
          groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x, pc.m_y+1)]);
        }
        if (m_groups_on_borders.find(PixelCoordinate(pc.m_x-1, pc.m_y+1)) != m_groups_on_borders.end()) {
          groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x-1, pc.m_y+1)]);
        }
        if (m_groups_on_borders.find(PixelCoordinate(pc.m_x+1, pc.m_y+1)) != m_groups_on_borders.end()) {
          groups_to_connect.emplace(m_groups_on_borders[PixelCoordinate(pc.m_x+1, pc.m_y+1)]);
        }
      }
    }

    if (groups_to_connect.size() > 0) {
      groups_to_connect.emplace(group);
      mergeGroups(groups_to_connect);
    }

    if (process_now) {
      publishSource(group);
    }
  }

  void publishSource(std::shared_ptr<std::vector<PixelCoordinate>> group) {
    auto source = m_source_factory->createSource();
    source->setProperty<PixelCoordinateList>(*group);
    source->setProperty<SourceId>();
    m_listener.publishSource(source);
  }

  void mergeGroups(std::set<std::shared_ptr<std::vector<PixelCoordinate>>> groups) {
    auto merged_group = std::make_shared<std::vector<PixelCoordinate>>();

    for (auto& group : groups) {
      merged_group->insert(merged_group->end(),
          std::make_move_iterator(group->begin()), std::make_move_iterator(group->end()));
    }

    for (auto& pc : *merged_group) {
      m_groups_on_borders[pc] = merged_group;
    }
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
    m_current_tile = pc;
    m_visited_tiles.emplace(pc);
  }

  void flush() {
    while (m_groups_on_borders.size() > 0) {
      auto group = m_groups_on_borders.begin()->second;
      publishSource(group);
      for (auto& pc : *group) {
        m_groups_on_borders.erase(pc);
      }
    }
  }

private:
  Segmentation::LabellingListener& m_listener;
  std::shared_ptr<SourceFactory> m_source_factory;

  std::unordered_map<PixelCoordinate, std::shared_ptr<std::vector<PixelCoordinate>>> m_groups_on_borders;

  int m_min_x, m_min_y;
  int m_max_x, m_max_y;

  PixelCoordinate m_current_tile;
  std::unordered_set<PixelCoordinate> m_visited_tiles;
};

}

void TileBasedSegmentation::labelImage(Segmentation::LabellingListener& listener,
    std::shared_ptr<const DetectionImageFrame> frame) {
  Lutz lutz;
  TilesLabellingListener tiles_listener(listener, m_source_factory);

  auto image = frame->getThresholdedImage();
  for (auto& tile : getTiles(*image)) {
    auto sub_image = SubImage<DetectionImage::PixelType>::create(image, tile.offset, tile.width, tile.height);
    tiles_listener.setTileBoundary(tile.offset, tile.width, tile.height);
    lutz.labelImage(tiles_listener, *sub_image, tile.offset);
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
        PixelCoordinate(x, y),
        std::min(tile_width, image.getWidth() - x),
        std::min(tile_height, image.getHeight() - y)
      });
    }
  }

  return tiles;
}


}
