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
#include "SEFramework/Source/SourceWithOnDemandProperties.h"
#include "SEFramework/Image/Image.h"
#include "SEImplementation/Plugin/ExternalFlag/ExternalFlagConfig.h"

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


} /* namespace SExtractor */


#endif
