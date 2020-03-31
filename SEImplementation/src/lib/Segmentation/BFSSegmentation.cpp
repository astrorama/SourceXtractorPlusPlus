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

#include <memory>
#include <vector>
#include <list>
#include <iostream>

#include "SEUtils/PixelCoordinate.h"
#include "SEUtils/HilbertCurve.h"

#include "SEFramework/Frame/Frame.h"
#include "SEFramework/Image/TileManager.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Property/SourceId.h"

#include "SEImplementation/Measurement/MultithreadedMeasurement.h"

#include "SEImplementation/Segmentation/BFSSegmentation.h"

namespace SourceXtractor {

void BFSSegmentation::labelImage(Segmentation::LabellingListener& listener,
                                 std::shared_ptr<const DetectionImageFrame> frame) {
  auto detection_image = frame->getThresholdedImage();
  auto tiles = getTiles(*detection_image);

  VisitedMap visited(detection_image->getWidth(), detection_image->getHeight());

  std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

  for (auto& tile : tiles) {
    for (int y=0; y<tile.height; y++) {
      for (int x=0; x<tile.width; x++) {
        PixelCoordinate pixel =  tile.offset + PixelCoordinate(x,y);
        if (!visited.wasVisited(pixel) && detection_image->getValue(pixel) > 0.0) {
          labelSource(pixel, listener, *detection_image, visited);
        }
      }
    }
  }
}

void BFSSegmentation::labelSource(PixelCoordinate pc,
                                  Segmentation::LabellingListener& listener,
                                  DetectionImage& detection_image,
                                  VisitedMap& visited_map) const {
  PixelCoordinate offsets[] {PixelCoordinate(1,0), PixelCoordinate(0,-1), PixelCoordinate(-1,0), PixelCoordinate(0,1),
      PixelCoordinate(-1,-1), PixelCoordinate(1,-1), PixelCoordinate(-1,1), PixelCoordinate(1,1)};

  std::vector<PixelCoordinate> source_pixels;
  std::vector<PixelCoordinate> pixels_to_process;

  visited_map.markVisited(pc);
  pixels_to_process.emplace_back(pc);

  while (pixels_to_process.size() > 0) {
    auto pixel = pixels_to_process.back();
    pixels_to_process.pop_back();
    source_pixels.emplace_back(pixel);

    for (auto& offset : offsets) {
      auto new_pixel = pixel + offset;
      if (!visited_map.wasVisited(new_pixel) && detection_image.getValue(new_pixel) > 0.0) {
        visited_map.markVisited(new_pixel);
        pixels_to_process.emplace_back(new_pixel);
      }
    }
  }

  auto source = m_source_factory->createSource();
  source->setProperty<PixelCoordinateList>(source_pixels);
  source->setProperty<SourceId>();
  listener.publishSource(source);
}

std::vector<BFSSegmentation::Tile> BFSSegmentation::getTiles(const DetectionImage& image) const {
  int tile_width = TileManager::getInstance()->getTileWidth();
  int tile_height = TileManager::getInstance()->getTileHeight();

  std::vector<BFSSegmentation::Tile> tiles;


  int size = std::max((image.getWidth() + tile_width - 1) / tile_width,
                      (image.getHeight() + tile_height - 1) / tile_height);

  HilbertCurve curve(size);

  for (auto& coord : curve.getCurve()) {
    int x = coord.m_x * tile_width;
    int y = coord.m_y * tile_height;

    if (x < image.getWidth() && y < image.getHeight()) {
      tiles.emplace_back(BFSSegmentation::Tile {
        PixelCoordinate(x, y),
        std::min(tile_width, image.getWidth() - x),
        std::min(tile_height, image.getHeight() - y)
      });
    }
  }

  return tiles;
}

}
