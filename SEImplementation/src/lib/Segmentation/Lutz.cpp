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
/*
 * Lutz.cpp
 *
 *  Created on: Jan 17, 2017
 *      Author: mschefer
 */


#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/TileManager.h"
#include "SEFramework/Source/SourceWithOnDemandProperties.h"

#include "SEImplementation/Measurement/MultithreadedMeasurement.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Segmentation/Lutz.h"

#include "SEImplementation/Segmentation/LutzSegmentation.h"

namespace SourceXtractor {

// class Lutz
//
//


enum class LutzStatus {
  COMPLETE = 0,
  INCOMPLETE,
  NONOBJECT,
  OBJECT
};

enum class LutzMarker {
  ZERO = 0,
  S,
  S0,
  F,
  F0
};


void Lutz::labelImage(LutzListener& listener, const DetectionImage& image, PixelCoordinate offset) {
  int width = image.getWidth() + 1; // one extra pixel

  std::vector<LutzMarker> marker(image.getWidth()+1);
  std::fill(marker.begin(), marker.end(), LutzMarker::ZERO);

  std::vector<PixelGroup> group_stack;
  std::vector<LutzStatus> ps_stack;

  std::unordered_map<int, PixelGroup> inc_group_map;
  //std::shared_ptr<VectorImage<unsigned int>> check_image=VectorImage<unsigned int>::create(image.getWidth(), image.getHeight());

  int lines = image.getHeight();
  int chunk_height = TileManager::getInstance()->getTileHeight();
  std::shared_ptr<ImageChunk<DetectionImage::PixelType>> chunk;

  for (int y = 0; y < lines; y++) {
    LutzStatus ps = LutzStatus::COMPLETE;
    LutzStatus cs = LutzStatus::NONOBJECT;

    if (y % chunk_height == 0) {
      std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);
      chunk = image.getChunk(0, y, image.getWidth(), std::min(chunk_height, lines - y));
    }

    int dy = y % chunk_height;
    for (int x=0; x < width; x++) {
      DetectionImage::PixelType value = (x == width - 1) ? 0.0 : chunk->getValue(x, dy);

      LutzMarker last_marker = marker[x];
      marker[x] = LutzMarker::ZERO;

      bool in_object = value > 0.0;
      if (in_object) {
        // We have an object pixel
        //check_image->setValue(x, y, 1);
        if (cs != LutzStatus::OBJECT) {
          // Previous pixel not object, start new segment

          cs = LutzStatus::OBJECT;

          if (ps == LutzStatus::OBJECT) {
            // Pixel touches segment on preceding scan

            if (group_stack.back().start == -1) {
              // First pixel of object on current scan
              marker[x] = LutzMarker::S;
              group_stack.back().start = x;
            } else {
              marker[x] = LutzMarker::S0;
            }
          } else {
            // Start of completely new pixel group
            ps_stack.push_back(ps);
            ps = LutzStatus::COMPLETE;
            group_stack.emplace_back();
            marker[x] = LutzMarker::S;
            group_stack.back().start = x;
          }
        }
      }

      if (last_marker != LutzMarker::ZERO) {
        // There is a marker from the previous scan to process
        // This is done for both object and non-object pixels

        if (last_marker == LutzMarker::S) {
          // Start of pixel group on preceding scan
          ps_stack.push_back(ps);
          if (cs == LutzStatus::NONOBJECT) {
            // The S marker is the first encounter with this group
            ps_stack.push_back(LutzStatus::COMPLETE);

            group_stack.emplace_back(std::move(inc_group_map.at(x)));
            inc_group_map.erase(x);

            group_stack.back().start = -1;
          } else {
            // Add group to current group
            auto prev_group = inc_group_map.at(x);
            inc_group_map.erase(x);

            group_stack.back().merge_pixel_list(prev_group);
          }
          ps = LutzStatus::OBJECT;
        }

        if (last_marker == LutzMarker::S0) {
          // Start of secondary segment of group on preceding scan

          if (cs == LutzStatus::OBJECT && ps == LutzStatus::COMPLETE) {
            // Current group is joined to preceding group
            ps_stack.pop_back();
            auto old_group = std::move(group_stack.back());
            group_stack.pop_back();
            group_stack.back().merge_pixel_list(old_group);

            if (group_stack.back().start == -1) {
              group_stack.back().start = old_group.start;
            } else {
              marker[old_group.start] = LutzMarker::S0;
            }
          }
          ps = LutzStatus::OBJECT;
        }

        if (last_marker == LutzMarker::F0) {
          ps = LutzStatus::INCOMPLETE;
        }

        if (last_marker == LutzMarker::F) {


          ps = ps_stack.back();
          ps_stack.pop_back();

          if (cs == LutzStatus::NONOBJECT && ps == LutzStatus::COMPLETE) {
            // If no more of current group to come then finish it
            auto old_group = std::move(group_stack.back());
            group_stack.pop_back();
            if (old_group.start == -1) {
              // Pixel group completed
              listener.publishGroup(old_group);
            } else {
              marker[old_group.end] = LutzMarker::F;
              inc_group_map[old_group.start] = old_group;
            }
            ps = ps_stack.back();
            ps_stack.pop_back();
          }
        }
      }

      if (in_object) {
        // Update current group by current pixel
        group_stack.back().pixel_list.push_back(PixelCoordinate(x, y) + offset);

      } else {
        // The current pixel is not object

        if (cs == LutzStatus::OBJECT) {
          // Previous pixel was object. Finish segment
          cs = LutzStatus::NONOBJECT;

          if (ps != LutzStatus::COMPLETE) {
            // End of segment but not necessarily of section
            marker[x] = LutzMarker::F0;
            group_stack.back().end = x;
          } else {
            // End of final segment of group section
            ps = ps_stack.back();
            ps_stack.pop_back();

            marker[x] = LutzMarker::F;

            auto old_group = group_stack.back();
            group_stack.pop_back();

            inc_group_map[old_group.start] = old_group;
          }
        }
      }
    }
    listener.notifyProgress(y + 1, lines);
  }

  //FitsWriter::writeFile<unsigned int>(*check_image, "segCheck.fits");
  // Process the pixel groups left in the inc_group_map
  for (auto& group : inc_group_map) {
    listener.publishGroup(group.second);
  }
}

void LutzList::publishGroup(PixelGroup& pixel_group) {
  m_groups.push_back(pixel_group);
}


}
