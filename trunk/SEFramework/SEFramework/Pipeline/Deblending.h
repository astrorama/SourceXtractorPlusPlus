/**
 * @file SEFramework/Pipeline/Deblending.h
 * @date 05/26/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_PIPELINE_DEBLENDING_H
#define _SEFRAMEWORK_PIPELINE_DEBLENDING_H

#include "SEUtils/Observable.h"
#include "SEFramework/Source/SourceGroup.h"
#include "SEFramework/Task/TaskProvider.h"

namespace SExtractor {

/**
 * @class DeblendAction
 * @brief An action to be applied to a SourceGroup for deblending its sources
 *
 */
class DeblendAction {
public:

  /// Destructor
  virtual ~DeblendAction() = default;

  // Performs the DeblendAction on the SourceGroup
  virtual void deblend(SourceGroup& group) const = 0;
};

/**
 * @class Deblending
 * @brief Performs a number of steps on a SourceGroup that can add or remove Sources
 * for deblending the group.
 *
 */
class Deblending : public Observer<std::shared_ptr<SourceGroup>>,
  public Observable<std::shared_ptr<SourceGroup>> {

public:

  /// Destructor
  virtual ~Deblending() = default;

  /// Constructor - takes a vector of DeblendAction to be applied, in order, to every SourceGroup
  Deblending(std::vector<std::shared_ptr<DeblendAction>> actions, std::shared_ptr<TaskProvider> task_provider);

  /// Handles a new SourceGroup, applies the DeblendActions and then notifies the observers with the result
  virtual void handleMessage(const std::shared_ptr<SourceGroup>& group) override;

private:
  std::vector<std::shared_ptr<DeblendAction>> m_actions;
  std::shared_ptr<TaskProvider> m_task_provider;

}; /* End of Deblending class */

} /* namespace SExtractor */


#endif
