/**
 * @file SEFramework/Pipeline/Deblending.h
 * @date 05/26/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_PIPELINE_DEBLENDING_H
#define _SEFRAMEWORK_PIPELINE_DEBLENDING_H

#include "SEUtils/Observable.h"
#include "SEFramework/Source/SourceList.h"
#include "SEFramework/Source/SourceGroup.h"
#include "SEFramework/Task/TaskProvider.h"

namespace SExtractor {

/**
 * @class DeblendAction
 * @brief An action to be applied to a SourceList
 *
 */
class DeblendAction {
public:

  /// Destructor
  virtual ~DeblendAction() = default;

  // Performs the DeblendAction on the SourceList
  virtual void deblend(SourceList& source_list) const = 0;
};

/**
 * @class Deblending
 * @brief Performs a number of steps on a SourceList that can add or remove Sources. Each finalized SourceList
 * is then converted to a SourceGroup.
 *
 */
class Deblending : public Observer<std::shared_ptr<SourceList>>,
  public Observable<std::shared_ptr<SourceGroup>> {

public:

  /// Destructor
  virtual ~Deblending() = default;

  /// Constructor - takes a vector of DeblendAction to be applied, in order, to every SourceList
  Deblending(std::vector<std::shared_ptr<DeblendAction>> actions, std::shared_ptr<TaskProvider> task_provider);

  /// Handles a new SourceList, applies the DeblendAction and then notifies the observers with an EntangledSourceGroup
  virtual void handleMessage(const std::shared_ptr<SourceList>& source_list) override;

private:
  std::vector<std::shared_ptr<DeblendAction>> m_actions;
  std::shared_ptr<TaskProvider> m_task_provider;

}; /* End of Deblending class */

} /* namespace SExtractor */


#endif
