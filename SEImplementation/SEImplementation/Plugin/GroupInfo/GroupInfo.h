/*
 * GroupInfo.h
 *
 *  Created on: 2019 M01 29
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_GROUPINFO_GROUPINFO_H_
#define _SEIMPLEMENTATION_PLUGIN_GROUPINFO_GROUPINFO_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

class GroupInfo : public Property {
public:

  GroupInfo(unsigned int group_id) : m_group_id(group_id) {}
  /**
   * @brief Destructor
   */
  virtual ~GroupInfo() = default;

  unsigned int getGroupId() const {
    return m_group_id;
  }

private:
  unsigned int m_group_id;
};

}


#endif /* _SEIMPLEMENTATION_PLUGIN_GROUPINFO_GROUPINFO_H_ */
