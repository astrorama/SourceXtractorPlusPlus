/*
 * AssocPartitionStep.h
 *
 *  Created on: Mar 18, 2021
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODEPARTITIONSTEP_H_
#define _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODEPARTITIONSTEP_H_

#include "SEFramework/Pipeline/Partition.h"

namespace SourceXtractor {

/**
 * @class MinAreaPartitionStep
 * @brief A PartitionStep that rejects Sources if they do not have at least a given number of pixels
 *
 */
class AssocModePartitionStep : public PartitionStep {

public:

  /// Destructor
  virtual ~AssocModePartitionStep() = default;

  /// Constructor
  AssocModePartitionStep(bool match_required);

  virtual std::vector<std::shared_ptr<SourceInterface>> partition(std::shared_ptr<SourceInterface> source) const override;


private:
  bool m_match_required;

}; /* End of AssocPartitionStep class */

} /* namespace SourceXtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODEPARTITIONSTEP_H_ */
