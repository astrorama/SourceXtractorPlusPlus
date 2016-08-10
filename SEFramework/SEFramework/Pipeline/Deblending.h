/**
 * @file SEFramework/Pipeline/Deblending.h
 * @date 05/26/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_PIPELINE_DEBLENDING_H
#define _SEFRAMEWORK_PIPELINE_DEBLENDING_H

#include "SEUtils/Observable.h"
#include "SEFramework/Source/SourceGroupInterface.h"
#include "SEFramework/Task/TaskProvider.h"

namespace SExtractor {

/**
 * @class DeblendStep
 * @brief An action to be applied to a SourceGroup for deblending its sources
 *
 */
class DeblendStep {
public:

  /// Destructor
  virtual ~DeblendStep() = default;

  // Performs the DeblendStep on the SourceGroup
  virtual void deblend(SourceGroupInterface& group) const = 0;
};

/**
 * @class Deblending
 * @brief Performs a number of steps on a SourceGroup that can add or remove Sources
 * for deblending the group.
 *
 */
class Deblending : public Observer<std::shared_ptr<SourceGroupInterface>>,
  public Observable<std::shared_ptr<SourceGroupInterface>> {

public:

  /// Destructor
  virtual ~Deblending() = default;

  /// Constructor - takes a vector of DeblendStep to be applied, in order, to every SourceGroup
  Deblending(std::vector<std::shared_ptr<DeblendStep>> deblend_steps);

  /// Handles a new SourceGroup, applies the DeblendSteps and then notifies the observers with the result
  virtual void handleMessage(const std::shared_ptr<SourceGroupInterface>& group) override;

private:
  std::vector<std::shared_ptr<DeblendStep>> m_deblend_steps;
}; /* End of Deblending class */

} /* namespace SExtractor */


#endif
