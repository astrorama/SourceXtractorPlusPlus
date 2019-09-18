/**
 * @file src/lib/Task/ExternalFlagTask.cpp
 * @date 06/17/16
 * @author nikoapos
 */

#include <mutex>

#include "SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Measurement/MultithreadedMeasurement.h"

#include "SEImplementation/Plugin/ExternalFlag/ExternalFlagTask.h"

namespace SExtractor {

template<typename Combine>
ExternalFlagTask<Combine>::ExternalFlagTask(std::shared_ptr<FlagImage> flag_image, unsigned int flag_instance)
  : m_flag_image(flag_image), m_flag_instance(flag_instance) {
}


template<typename Combine>
void ExternalFlagTask<Combine>::computeProperties(SourceInterface &source) const {
  std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

  const auto& detection_frame = source.getProperty<DetectionFrame>();
  const auto& detection_image = detection_frame.getFrame()->getOriginalImage();

  if (m_flag_image->getWidth() != detection_image->getWidth() || m_flag_image->getHeight() != detection_image->getHeight()) {
    throw Elements::Exception()
      << "The flag image size does not match the detection image size: "
      << m_flag_image->getWidth() << "x" << m_flag_image->getHeight() << " != "
      << detection_image->getWidth() << "x" << detection_image->getHeight();
  }

  std::vector<FlagImage::PixelType> pixel_flags{};
  for (auto& coords : source.getProperty<PixelCoordinateList>().getCoordinateList()) {
    pixel_flags.push_back(m_flag_image->getValue(coords.m_x, coords.m_y));
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

} // SExtractor namespace



