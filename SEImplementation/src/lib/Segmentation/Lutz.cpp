/**
 * @file src/lib/Segmentation/Lutz.cpp
 * @date 05/31/16
 * @author mschefer
 */

#include "SEFramework/Image/Image.h"
#include "SEFramework/Source/Source.h"

#include "SEImplementation/Segmentation/Lutz.h"


namespace SExtractor {

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

class PixelGroup {
public:

  int start;
  int end;
  std::vector<PixelCoordinate> pixel_list;

  PixelGroup() : start(-1), end(-1) {}

  void merge_pixel_list(PixelGroup& other) {
    pixel_list.insert(pixel_list.end(), other.pixel_list.begin(), other.pixel_list.end());
  }
};

void Lutz::publishGroup(PixelGroup& pixel_group) {
  auto source = std::make_shared<Source>(pixel_group.pixel_list, m_task_registry);
  notifyObservers(source);
}

void Lutz::scan(const DetectionImage& image) {

  int width = image.getWidth() + 1; // one extra pixel

  std::vector<LutzMarker> marker(image.getWidth()+1);
  std::fill(marker.begin(), marker.end(), LutzMarker::ZERO);

  std::vector<PixelGroup> group_stack;
  std::vector<LutzStatus> ps_stack;

  std::unordered_map<int, PixelGroup> inc_group_map;

  for (int y=0; y<image.getHeight(); y++) {
    LutzStatus ps = LutzStatus::COMPLETE;
    LutzStatus cs = LutzStatus::NONOBJECT;


    for (int x=0; x < width; x++) {
      double value = (x == width - 1) ? 0.0 : image.getValue(x, y);


      LutzMarker last_marker = marker[x];
      marker[x] = LutzMarker::ZERO;

      bool in_object = value > 0.0;
      if (in_object) {
        // We have an object pixel

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
            group_stack.push_back(PixelGroup());
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

            group_stack.push_back(inc_group_map.at(x));
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
            auto old_group = group_stack.back();
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
            auto old_group = group_stack.back();
            group_stack.pop_back();
            if (old_group.start == -1) {
              // Pixel group completed
              publishGroup(old_group);
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
        group_stack.back().pixel_list.push_back(PixelCoordinate(x, y));

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
  }

  // Process the pixel groups left in the inc_group_map
  for (auto& group : inc_group_map) {
    publishGroup(group.second);
  }
}

} // Segmentation namespace



