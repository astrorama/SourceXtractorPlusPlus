/*
 * DetectionNeighbourInfoTask.cpp
 *
 *  Created on: Oct 02, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionNeighbourInfo/DetectionNeighbourInfoTask.h"
#include "SEImplementation/Plugin/DetectionNeighbourInfo/DetectionNeighbourInfo.h"

namespace SExtractor {

void DetectionNeighbourInfoTask::computeProperties(SExtractor::SourceInterface &source) const {
  const auto &detection_frame = source.getProperty<DetectionFrame>().getFrame();
  const auto &detection_pixel_list = source.getProperty<PixelCoordinateList>().getCoordinateList();
  const auto &detection_boundaries = source.getProperty<PixelBoundaries>();

  const auto &detection_threshold = detection_frame->getThresholdedImage();

  const auto &detection_pixel_min = detection_boundaries.getMin();

  auto detection_neighbour_img = VectorImage<int>::create(detection_boundaries.getWidth(),
                                                          detection_boundaries.getHeight());

  for (auto pixel_coord : detection_pixel_list) {
    auto act_x = pixel_coord.m_x - detection_pixel_min.m_x;
    auto act_y = pixel_coord.m_y - detection_pixel_min.m_y;

    // make sure to be inside the image
    assert(act_x >= 0 && act_y >= 0 && act_x < detection_neighbour_img->getWidth() &&
           act_y < detection_neighbour_img->getHeight());
    detection_neighbour_img->setValue(act_x, act_y, -1);
  }

  // Set surrounding pixels above the threshold to 1, all others to 0
  for (int act_y = 0; act_y < detection_neighbour_img->getHeight(); act_y++) {
    for (int act_x = 0; act_x < detection_neighbour_img->getWidth(); act_x++) {
      int dx = act_x + detection_pixel_min.m_x;
      int dy = act_y + detection_pixel_min.m_y;

      if (detection_threshold->getValue(dx, dy) > 0 && detection_neighbour_img->getValue(act_x, act_y) != -1) {
        detection_neighbour_img->setValue(act_x, act_y, 1);
      } else {
        detection_neighbour_img->setValue(act_x, act_y, 0);
      }
    }
  }

  source.setProperty<DetectionNeighbourInfo>(detection_pixel_min, detection_neighbour_img);
}

} // end SExtractor
