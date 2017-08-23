/**
 * @file SEImplementation/Task/ExternalFlagTask.h
 * @date 06/17/16
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_TASK_EXTERNALFLAGTASK_H
#define _SEIMPLEMENTATION_TASK_EXTERNALFLAGTASK_H

#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Source/SourceWithOnDemandProperties.h"
#include "SEImplementation/Plugin/ExternalFlag/ExternalFlag.h"

namespace SExtractor {

namespace ExternalFlagCombineTypes {
class Or;
class And;
class Min;
class Max;
class Most;
}

/**
 * @class ExternalFlagTask
 * @brief Task to produce the ExternalFlag Property
 *
 */
template <typename Combine>
class ExternalFlagTask : public SourceTask {
  
public:
  
  virtual ~ExternalFlagTask() = default;
  
  ExternalFlagTask(std::shared_ptr<FlagImage> flag_image, unsigned int flag_instance);

  void computeProperties(SourceInterface& source) const override;

private:
  
  std::shared_ptr<FlagImage> m_flag_image;
  unsigned int m_flag_instance;
  
};

extern template class ExternalFlagTask<ExternalFlagCombineTypes::Or>;
using ExternalFlagTaskOr = ExternalFlagTask<ExternalFlagCombineTypes::Or>;

extern template class ExternalFlagTask<ExternalFlagCombineTypes::And>;
using ExternalFlagTaskAnd = ExternalFlagTask<ExternalFlagCombineTypes::And>;

extern template class ExternalFlagTask<ExternalFlagCombineTypes::Min>;
using ExternalFlagTaskMin = ExternalFlagTask<ExternalFlagCombineTypes::Min>;

extern template class ExternalFlagTask<ExternalFlagCombineTypes::Max>;
using ExternalFlagTaskMax = ExternalFlagTask<ExternalFlagCombineTypes::Max>;

extern template class ExternalFlagTask<ExternalFlagCombineTypes::Most>;
using ExternalFlagTaskMost = ExternalFlagTask<ExternalFlagCombineTypes::Most>;

} /* namespace SExtractor */


#endif
