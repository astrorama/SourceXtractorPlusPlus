/*
 * MultiThresholdPartitionStep.h
 *
 *  Created on: Jan 17, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PARTITION_MULTITHRESHOLDPARTITIONSTEP_H_
#define _SEIMPLEMENTATION_PARTITION_MULTITHRESHOLDPARTITIONSTEP_H_

#include "SEFramework/Pipeline/Partition.h"
#include "SEFramework/Source/SourceFactory.h"

namespace SExtractor {

/**
 * @class MinAreaPartitionStep
 * @brief A PartitionStep that rejects Sources if they do not have at least a given number of pixels
 *
 */
class MultiThresholdPartitionStep : public PartitionStep {

public:

  MultiThresholdPartitionStep(std::shared_ptr<SourceFactory> source_factory) :
    m_source_factory(source_factory) {}

  virtual ~MultiThresholdPartitionStep() = default;

  virtual std::vector<std::shared_ptr<SourceInterface>> partition(std::shared_ptr<SourceInterface> source) const;


private:
  std::shared_ptr<SourceFactory> m_source_factory;
};


}




#endif /* _SEIMPLEMENTATION_PARTITION_MULTITHRESHOLDPARTITIONSTEP_H_ */
