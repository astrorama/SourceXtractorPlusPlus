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
#include "SEFramework/Image/ImageAccessor.h"
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
    auto chunk = detection_image->getChunk(tile.offset.m_x, tile.offset.m_y, tile.width, tile.height);
    for (int y=0; y<tile.height; y++) {
      for (int x=0; x<tile.width; x++) {
        PixelCoordinate pixel =  tile.offset + PixelCoordinate(x,y);
        if (!visited.wasVisited(pixel) && chunk->getValue(x, y) > 0.0) {
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
  using DetectionAccessor = ImageAccessor<DetectionImage::PixelType>;
  DetectionAccessor detectionAccessor(detection_image);

  PixelCoordinate offsets[] {PixelCoordinate(1,0), PixelCoordinate(0,-1), PixelCoordinate(-1,0), PixelCoordinate(0,1),
      PixelCoordinate(-1,-1), PixelCoordinate(1,-1), PixelCoordinate(-1,1), PixelCoordinate(1,1)};

  std::vector<PixelCoordinate> source_pixels;
  std::vector<PixelCoordinate> pixels_to_process;

  visited_map.markVisited(pc);
  pixels_to_process.emplace_back(pc);

  PixelCoordinate minPixel = pc;
  PixelCoordinate maxPixel = pc;

  while (pixels_to_process.size() > 0) {
    auto pixel = pixels_to_process.back();
    pixels_to_process.pop_back();
    source_pixels.emplace_back(pixel);

    minPixel.m_x = std::min(minPixel.m_x, pixel.m_x);
    minPixel.m_y = std::min(minPixel.m_y, pixel.m_y);
    maxPixel.m_x = std::max(maxPixel.m_x, pixel.m_x);
    maxPixel.m_y = std::max(maxPixel.m_y, pixel.m_y);

    if (maxPixel.m_x - minPixel.m_x > m_max_delta || maxPixel.m_y - minPixel.m_y > m_max_delta) {
      // The source extends over a too large area, ignore it
      return;
    }

    for (auto& offset : offsets) {
      auto new_pixel = pixel + offset;

      if (!visited_map.wasVisited(new_pixel) && detectionAccessor.getValue(new_pixel) > 0.0) {
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
