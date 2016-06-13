/**
 * @file SEImplementation/Partition/MinAreaPartitionStep.h
 * @date 05/30/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PARTITION_MINAREAPARTITIONSTEP_H
#define _SEIMPLEMENTATION_PARTITION_MINAREAPARTITIONSTEP_H

#include "SEFramework/Source/Source.h"
#include "SEFramework/Pipeline/Partition.h"

namespace SExtractor {

/**
 * @class MinAreaPartitionStep
 * @brief
 *
 */
class MinAreaPartitionStep : public PartitionStep {

public:

  /**
   * @brief Destructor
   */
  virtual ~MinAreaPartitionStep() = default;

  MinAreaPartitionStep(unsigned int min_pixel_count);

  virtual std::vector<std::shared_ptr<Source>> partition(
      std::shared_ptr<Source> source) const override;


private:
  unsigned int m_min_pixel_count;


}; /* End of MinAreaPartitionStep class */

} /* namespace SEImplementation */


#endif
