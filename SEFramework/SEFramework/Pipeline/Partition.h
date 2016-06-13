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
 * @brief
 *
 */
class Partition : public Observer<std::shared_ptr<Source>>,
    public Observable<std::shared_ptr<Source>> {

public:

  /**
   * @brief Destructor
   */
  virtual ~Partition() = default;

  Partition(std::vector<std::shared_ptr<PartitionStep>> steps);

  virtual void handleMessage(const std::shared_ptr<Source>& source) override;

private:
  std::vector<std::shared_ptr<PartitionStep>> m_steps;

}; /* End of Partition class */

} /* namespace SEFramework */


#endif
