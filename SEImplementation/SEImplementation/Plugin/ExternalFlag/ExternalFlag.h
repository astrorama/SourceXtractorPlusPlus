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

namespace SourceXtractor {

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


} /* namespace SourceXtractor */


#endif
