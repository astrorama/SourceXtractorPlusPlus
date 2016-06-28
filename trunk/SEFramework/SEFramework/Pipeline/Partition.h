/**
 * @file Overall/Pipeline/PixelRefinement.h
 * @date 05/23/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_PIPELINE_PARTITION_H
#define _SEFRAMEWORK_PIPELINE_PARTITION_H

#include "SEUtils/Observable.h"
#include "SEFramework/Source/Source.h"

namespace SExtractor {

/**
 * @class PartitionStep
 * @brief A PartitionStep gets applied on a single Source and can result any number of Sources being outputed.
 *  (including the same one, none, or any number of new Sources)
 *
 */
class PartitionStep {
public:

  /**
   * @brief Destructor
   */
  virtual ~PartitionStep() = default;

  virtual std::vector<std::shared_ptr<Source>> partition(std::shared_ptr<Source> source) const = 0;
};

/**
 * @class Partition
 * @brief For each Source it receives, applies PartitionSteps and outputs one or more Sources.
 *
 * @details The first PartitionStep is applied to the Source that was received. Each subsequent PartitionStep
 * is applied to the Source(s) produced by the previous step. The Sources resulting from the last step are
 * notified to the Observers one by one.
 *
 */
class Partition : public Observer<std::shared_ptr<Source>>, public Observable<std::shared_ptr<Source>> {

public:

  /**
   * @brief Destructor
   */
  virtual ~Partition() = default;

  /// Constructor - takes a vector of PartitionSteps to be applied in order
  Partition(std::vector<std::shared_ptr<PartitionStep>> steps);

  /// Handles a Source (applies PartitionSteps) and notifies the Observers for every Source in the final result
  virtual void handleMessage(const std::shared_ptr<Source>& source) override;

private:
  std::vector<std::shared_ptr<PartitionStep>> m_steps;

}; /* End of Partition class */

} /* namespace SExtractor */


#endif
