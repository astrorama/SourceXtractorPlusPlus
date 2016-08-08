/**
 * @file SEImplementation/Partition/MinAreaPartitionStep.h
 * @date 05/30/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PARTITION_MINAREAPARTITIONSTEP_H
#define _SEIMPLEMENTATION_PARTITION_MINAREAPARTITIONSTEP_H

#include "SEFramework/Pipeline/Partition.h"

namespace SExtractor {

/**
 * @class MinAreaPartitionStep
 * @brief A PartitionStep that rejects Sources if they do not have at least a given number of pixels
 *
 */
class MinAreaPartitionStep : public PartitionStep {

public:

  /// Destructor
  virtual ~MinAreaPartitionStep() = default;

  /// Constructor
  MinAreaPartitionStep(unsigned int min_pixel_count);

  virtual std::vector<std::shared_ptr<SourceInterface>> partition(std::shared_ptr<SourceInterface> source) const override;


private:
  unsigned int m_min_pixel_count;


}; /* End of MinAreaPartitionStep class */

} /* namespace SExtractor */


#endif
