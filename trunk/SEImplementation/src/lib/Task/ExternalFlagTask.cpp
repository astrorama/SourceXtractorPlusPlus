/**
 * @file src/lib/Task/ExternalFlagTask.cpp
 * @date 06/17/16
 * @author nikoapos
 */

#include "SEFramework/Property/PixelCoordinateList.h"
#include "SEImplementation/Task/ExternalFlagTask.h"

namespace SExtractor {

template<typename Combine>
ExternalFlagTask<Combine>::ExternalFlagTask(std::shared_ptr<FlagImage> flag_image, unsigned int flag_instance)
        : m_flag_image(flag_image), m_flag_instance(flag_instance) {
}


template<typename Combine>
void ExternalFlagTask<Combine>::computeProperties(SourceInterface& source) const {
  std::vector<FlagImage::PixelType> pixel_flags{};
  for (auto& coords : source.getProperty<PixelCoordinateList>().getCoordinateList()) {
    pixel_flags.push_back(m_flag_image->getValue(coords.m_x, coords.m_y));
  }
  std::int64_t flag = 0;
  int count = 0;
  std::tie(flag, count) = Combine::combine(pixel_flags);
  source.setProperty<ExternalFlag>(
          std::unique_ptr<ExternalFlag>(new ExternalFlag{flag, count}),
          m_flag_instance
  );
}


namespace ExternalFlagCombineTypes {

struct Or {
  static std::pair<std::int64_t, int> combine(const std::vector<FlagImage::PixelType>& pixel_flags) {
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
  static std::pair<std::int64_t, int> combine(const std::vector<FlagImage::PixelType>& pixel_flags) {
    std::int64_t flag = std::numeric_limits<std::int64_t>::max();
    int count = pixel_flags.size();
    for (auto pix_flag : pixel_flags) {
      flag &= pix_flag;
    }
    return {flag, count};
  }
};

struct Min {
  static std::pair<std::int64_t, int> combine(const std::vector<FlagImage::PixelType>& pixel_flags) {
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
  static std::pair<std::int64_t, int> combine(const std::vector<FlagImage::PixelType>& pixel_flags) {
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
  static std::pair<std::int64_t, int> combine(const std::vector<FlagImage::PixelType>& pixel_flags) {
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



