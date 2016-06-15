/**
 * @file SEImplementation/Property/ExternalFlag.h
 * @date 06/13/16
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_EXTERNALFLAG_H
#define _SEIMPLEMENTATION_PROPERTY_EXTERNALFLAG_H

#include <cstdint>
#include <memory>
#include <limits>

#include "Configuration/ConfigManager.h"

#include "SEFramework/Property/Property.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Task/TaskFactory.h"
#include "SEFramework/Source/Source.h"
#include "SEFramework/Image/Image.h"
#include "SEConfig/ExternalFlagConfig.h"

namespace SExtractor {

/**
 * @class ExternalFlag
 * @brief
 *
 */
class ExternalFlag : public Property {

public:

  /**
   * @brief Destructor
   */
  virtual ~ExternalFlag() = default;

  ExternalFlag(std::int64_t flag, int count) : m_flag(flag), m_count(count) { }
  
  int getCount() const {
    return m_count;
  }

  std::int64_t getFlag() const {
    return m_flag;
  }

private:
  
  std::int64_t m_flag;
  int m_count;

}; /* End of ExternalFlag class */

template <typename Combine>
class ExternalFlagTask : public SourceTask {
  
public:
  
  virtual ~ExternalFlagTask() = default;
  
  ExternalFlagTask(std::shared_ptr<FlagImage> flag_image, const std::string& flag_name)
        : m_flag_image(flag_image), m_flag_name(flag_name) { }

  void computeProperties(Source& source) const override {
    std::vector<FlagImage::PixelType> pixel_flags {};
    for (auto& coords : source.getPixels()) {
      pixel_flags.push_back(m_flag_image->getValue(coords.m_x, coords.m_y));
    }
    std::int64_t flag = 0;
    int count = 0;
    std::tie(flag, count) = Combine::combine(pixel_flags);
    source.setProperty(std::unique_ptr<ExternalFlag>(new ExternalFlag{flag, count}), m_flag_name);
  }

private:
  
  std::shared_ptr<FlagImage> m_flag_image;
  std::string m_flag_name;
  
};

class ExternalFlagTaskFactory : public TaskFactory {
  
public:
  
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
  
  ExternalFlagTaskFactory() {
    auto& conf_mgr = Euclid::Configuration::ConfigManager::getInstance(0);
    auto& flag_info_map = conf_mgr.getConfiguration<ExternalFlagConfig>().getFlagInfo();
    for (auto& pair : flag_info_map) {
      switch (pair.second.second) {
        case ExternalFlagConfig::Type::OR:
          m_task_map[pair.first] = std::shared_ptr<SourceTask> {
            new ExternalFlagTask<Or>(pair.second.first, pair.first)
          };
          break;
        case ExternalFlagConfig::Type::AND:
          m_task_map[pair.first] = std::shared_ptr<SourceTask> {
            new ExternalFlagTask<And>(pair.second.first, pair.first)
          };
          break;
        case ExternalFlagConfig::Type::MIN:
          m_task_map[pair.first] = std::shared_ptr<SourceTask> {
            new ExternalFlagTask<Min>(pair.second.first, pair.first)
          };
          break;
        case ExternalFlagConfig::Type::MAX:
          m_task_map[pair.first] = std::shared_ptr<SourceTask> {
            new ExternalFlagTask<Max>(pair.second.first, pair.first)
          };
          break;
        case ExternalFlagConfig::Type::MOST:
          m_task_map[pair.first] = std::shared_ptr<SourceTask> {
            new ExternalFlagTask<Most>(pair.second.first, pair.first)
          };
          break;
      }
    }
  }
  
  virtual ~ExternalFlagTaskFactory() = default;

  virtual std::shared_ptr<Task> getTask(PropertyId property_id) override {
    if (property_id.m_property_type != typeid(ExternalFlag)) {
      return nullptr;
    }
    auto& name = property_id.m_parameter;
    if (m_task_map.count(name) != 1) {
      return nullptr;
    }
    return m_task_map.at(name);
  }
  
  virtual const std::vector<PropertyId> getProducedProperties() override {
    return {PropertyId(typeid(ExternalFlag), "top"), PropertyId(typeid(ExternalFlag), "points")};
  }
  
private:
  
  std::map<std::string, std::shared_ptr<SourceTask>> m_task_map;
  
};

} /* namespace SExtractor */


#endif
