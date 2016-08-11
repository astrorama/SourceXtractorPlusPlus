/**
 * @file src/lib/Partition/AttractorsPartitionStep.cpp
 * @date 06/03/16
 * @author mschefer
 */
#include <limits>

#include "SEFramework/Image/Image.h"
#include "SEFramework/History/SourceHistory.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStamp.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Partition/AttractorsPartitionStep.h"

namespace SExtractor {

std::vector<std::shared_ptr<SourceInterface>> AttractorsPartitionStep::partition(std::shared_ptr<SourceInterface> source) const {
  auto& stamp = source->getProperty<DetectionFrameSourceStamp>().getStamp();
  auto& bounds = source->getProperty<PixelBoundaries>();

  auto bbox_min = bounds.getMin();
  auto bbox_max = bounds.getMax();

  auto value_function = [bbox_min, bbox_max, &stamp](PixelCoordinate coord) {
    if (coord.m_x < bbox_min.m_x || coord.m_x > bbox_max.m_x || coord.m_y < bbox_min.m_y || coord.m_y > bbox_max.m_y) {
      return std::numeric_limits<double>::lowest();
    }
    auto offset_coord = coord - bbox_min;
    return stamp.getValue(offset_coord.m_x, offset_coord.m_y);
  };

  std::vector<std::pair<PixelCoordinate, PixelCoordinate>> pixel_coordinates;
  auto& pixel_list = source->getProperty<PixelCoordinateList>().getCoordinateList();
  pixel_coordinates.reserve(pixel_list.size());
  for (auto& pixel : pixel_list) {
    pixel_coordinates.emplace_back(pixel, pixel);
  }

  std::unordered_map<PixelCoordinate, std::vector<PixelCoordinate>> attractors;

  attractPixels(pixel_coordinates, attractors, value_function);
  auto merged = mergeAttractors(attractors);

  // If we end up with a single group use the original group
  if (merged.size() == 1) {
    return { source };
  } else {
    std::vector<std::shared_ptr<SourceInterface>> sources;
    for (auto& source_pixels : merged) {
      auto new_source = m_source_factory->createSource();
      new_source->setProperty<PixelCoordinateList>(source_pixels);
      // FIXME temporarily disabling History functionality
      //new_source->addHistoryEntry(std::unique_ptr<HistoryEntry>(new SourceHistory(source)));
      sources.push_back(new_source);
    }
    return sources;
  }
}

void AttractorsPartitionStep::attractPixels(
    const std::vector<std::pair<PixelCoordinate, PixelCoordinate>>& pixels_with_origin,
    std::unordered_map<PixelCoordinate, std::vector<PixelCoordinate>>& attractors,
    std::function<double (PixelCoordinate)> value_function) const {

  PixelCoordinate offsets[5] {
    PixelCoordinate( 0,  0),
    PixelCoordinate(-1,  0),
    PixelCoordinate( 0, -1),
    PixelCoordinate( 1,  0),
    PixelCoordinate( 0,  1)
  };

  if (pixels_with_origin.size() == 0) {
    return;
  }

  std::vector<std::pair<PixelCoordinate, PixelCoordinate>> pixels_to_be_processed;

  for (auto& pixel_origin : pixels_with_origin) {
    auto pixel = pixel_origin.first;
    auto origin = pixel_origin.second;

    double values[5];
    for (int i=0; i<5; i++) {
      values[i] = value_function(pixel + offsets[i]);
    }

    size_t max = 0;
    for (int i=1; i<3; i++) {
      if (values[i] > values[max]) {
        max = i;
      }
    }
    for (int i=3; i<5; i++) {
      if (values[i] >= values[max]) {
        max = i;
      }
    }

    if (max == 0) {
      // We are at the attractor pixel
      attractors[pixel].push_back(origin);
    } else {
      pixels_to_be_processed.push_back(
          std::pair<PixelCoordinate, PixelCoordinate>(pixel + offsets[max], origin));
    }
  }
  attractPixels(pixels_to_be_processed, attractors, value_function);
}

std::vector<std::vector<PixelCoordinate>> AttractorsPartitionStep::mergeAttractors(
    const std::unordered_map<PixelCoordinate, std::vector<PixelCoordinate>>& attractors) const {
  std::vector<std::vector<PixelCoordinate>> merged;
  std::vector<PixelCoordinate> bbox_min;
  std::vector<PixelCoordinate> bbox_max;

  for (auto& attractor : attractors) {
    auto coord = attractor.first;
    auto& pixels = attractor.second;
    bool done = false;

    for (size_t i=0; i < merged.size(); i++) {
      if (coord.m_x >= bbox_min[i].m_x-1 && coord.m_x <= bbox_max[i].m_x+1 && coord.m_y >= bbox_min[i].m_y-1 && coord.m_y <= bbox_max[i].m_y+1) {
        bbox_min[i] = PixelCoordinate(std::min(coord.m_x, bbox_min[i].m_x), std::min(coord.m_y, bbox_min[i].m_y));
        bbox_max[i] = PixelCoordinate(std::max(coord.m_x, bbox_max[i].m_x), std::max(coord.m_y, bbox_max[i].m_y));
        merged[i].insert(merged[i].begin(), pixels.begin(), pixels.end());
        done = true;
        break;
      }
    }
    if (!done) {
      merged.push_back(pixels);
      bbox_min.push_back(coord);
      bbox_max.push_back(coord);
    }
  }
  return merged;
}

} // SEImplementation namespace



