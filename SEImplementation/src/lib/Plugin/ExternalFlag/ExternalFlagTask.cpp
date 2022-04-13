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
/**
 * @file src/lib/Task/ExternalFlagTask.cpp
 * @date 06/17/16
 * @author nikoapos
 */

#include <mutex>

#include "SEFramework/Image/ImageAccessor.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFrameInfo/DetectionFrameInfo.h"

#include "SEImplementation/Plugin/ExternalFlag/ExternalFlagTask.h"

namespace SourceXtractor {

template<typename Combine>
ExternalFlagTask<Combine>::~ExternalFlagTask() {
}

template<typename Combine>
ExternalFlagTask<Combine>::ExternalFlagTask(const std::vector<std::shared_ptr<FlagImage>>& flag_images,
                                            unsigned int flag_instance)
  : m_flag_images(flag_images),
    m_flag_instance(flag_instance) {
}


template<typename Combine>
void ExternalFlagTask<Combine>::computeProperties(SourceInterface &source) const {
  // FIXME: for flag_image access, the external flags image not part of detection frame?!
  const auto& detection_frame_info = source.getProperty<DetectionFrameInfo>();

  auto flag_image_acc = ImageAccessor<int64_t>(m_flag_images.at(detection_frame_info.getHduIndex()));

  if (flag_image_acc.getWidth() != detection_frame_info.getWidth() ||
      flag_image_acc.getHeight() != detection_frame_info.getHeight()) {
    throw Elements::Exception()
      << "The flag image size does not match the detection image size: "
      << flag_image_acc.getWidth() << "x" << flag_image_acc.getHeight() << " != "
      << detection_frame_info.getWidth() << "x" << detection_frame_info.getHeight();
  }

  std::vector<FlagImage::PixelType> pixel_flags{};
  for (auto& coords : source.getProperty<PixelCoordinateList>().getCoordinateList()) {
    pixel_flags.push_back(flag_image_acc.getValue(coords.m_x, coords.m_y));
  }
  std::int64_t flag = 0;
  int count = 0;
  std::tie(flag, count) = Combine::combine(pixel_flags);
  source.setIndexedProperty<ExternalFlag>(m_flag_instance, flag, count);
}


namespace ExternalFlagCombineTypes {

struct Or {
  static std::pair<std::int64_t, int> combine(const std::vector<FlagImage::PixelType> &pixel_flags) {
    std::int64_t flag = 0;
    int count = 0;
    for (auto pix_flag : pixel_flags) {
      if (pix_flag != 0) {
        flag |= pix_flag;
        ++count;
      }
    }
    return {flag, count};
  }
};

struct And {
  static std::pair<std::int64_t, int> combine(const std::vector<FlagImage::PixelType> &pixel_flags) {
    std::int64_t flag = std::numeric_limits<std::int64_t>::max();
    int count = pixel_flags.size();
    for (auto pix_flag : pixel_flags) {
      flag &= pix_flag;
    }
    return {flag, count};
  }
};

struct Min {
  static std::pair<std::int64_t, int> combine(const std::vector<FlagImage::PixelType> &pixel_flags) {
    std::int64_t flag = std::numeric_limits<std::int64_t>::max();
    int count = 0;
    for (auto pix_flag : pixel_flags) {
      if (pix_flag < flag) {
        flag = pix_flag;
        count = 1;
      } else if (pix_flag == flag) {
        ++count;
      }
    }
    if (count == 0) {
      flag = 0;
    }
    return {flag, count};
  }
};

struct Max {
  static std::pair<std::int64_t, int> combine(const std::vector<FlagImage::PixelType> &pixel_flags) {
    std::int64_t flag = 0;
    int count = 0;
    for (auto pix_flag : pixel_flags) {
      if (pix_flag > flag) {
        flag = pix_flag;
        count = 1;
      } else if (pix_flag == flag) {
        ++count;
      }
    }
    if (count == 0) {
      flag = 0;
    }
    return {flag, count};
  }
};

struct Most {
  static std::pair<std::int64_t, int> combine(const std::vector<FlagImage::PixelType> &pixel_flags) {
    std::map<FlagImage::PixelType, int> counters;
    for (auto pix_flag : pixel_flags) {
      counters[pix_flag] += 1;
    }
    std::int64_t flag = 0;
    int count = 0;
    for (auto& pair : counters) {
      if (pair.second > count) {
        flag = pair.first;
        count = pair.second;
      }
    }
    return {flag, count};
  }
};

} // end of namespace ExternalFlagCombineTypes

template class ExternalFlagTask<ExternalFlagCombineTypes::Or>;
template class ExternalFlagTask<ExternalFlagCombineTypes::And>;
template class ExternalFlagTask<ExternalFlagCombineTypes::Min>;
template class ExternalFlagTask<ExternalFlagCombineTypes::Max>;
template class ExternalFlagTask<ExternalFlagCombineTypes::Most>;

} // SourceXtractor namespace



