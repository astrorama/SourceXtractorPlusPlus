/**
 * @file SEFramework/Pipeline/Deblending.h
 * @date 05/26/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_PIPELINE_DEBLENDING_H
#define _SEFRAMEWORK_PIPELINE_DEBLENDING_H

#include "SEUtils/Observable.h"
#include "SEFramework/Source/SourceList.h"
#include "SEFramework/Source/EntangledSourceGroup.h"
#include "SEFramework/Task/TaskRegistry.h"

namespace SEFramework {

class DeblendAction {
public:

  /**
   * @brief Destructor
   */
  virtual ~DeblendAction() = default;

  virtual void deblend(SourceList& source_list) const = 0;
};

/**
 * @class Deblending
 * @brief
 *
 */
class Deblending : public SEUtils::Observer<std::shared_ptr<SourceList>>,
  public SEUtils::Observable<std::shared_ptr<EntangledSourceGroup>> {

public:

  /**
   * @brief Destructor
   */
  virtual ~Deblending() = default;

  Deblending(std::vector<std::shared_ptr<DeblendAction>> actions, std::shared_ptr<TaskRegistry> task_registry);

  virtual void handleMessage(const std::shared_ptr<SourceList>& source_list) override;


private:
  std::vector<std::shared_ptr<DeblendAction>> m_actions;
  std::shared_ptr<TaskRegistry> m_task_registry;

}; /* End of Deblending class */

} /* namespace SEFramework */


#endif
