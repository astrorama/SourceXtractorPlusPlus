/*
 * GroupInfoTask.h
 *
 *  Created on: 2019 M01 29
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_GROUPINFO_GROUPINFOTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_GROUPINFO_GROUPINFOTASK_H_

#include "SEFramework/Task/GroupTask.h"

namespace SExtractor {

/**
 * @class GroupInfoTask
 * @brief Task to produce the GroupInfo Property
 *
 */

class GroupInfoTask : public GroupTask {
public:

  /**
   * @brief Destructor
   */
  virtual ~GroupInfoTask() = default;
  GroupInfoTask() {}

  virtual void computeProperties(SourceGroupInterface& group) const override;

private:

}; /* End of GroupInfoTask class */


} /* namespace SExtractor */



#endif /* _SEIMPLEMENTATION_PLUGIN_GROUPINFO_GROUPINFOTASK_H_ */
