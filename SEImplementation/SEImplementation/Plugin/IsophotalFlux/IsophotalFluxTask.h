/**
 * @file SEImplementation/IsophotalFluxTask.h
 * @date 09/28/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_TASK_ISOPHOTALFLUXTASK_H
#define _SEIMPLEMENTATION_TASK_ISOPHOTALFLUXTASK_H

#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

/**
 * @class IsophotalFluxTask
 * @brief Task to produce the IsophotalFlux Property
 *
 */

class IsophotalFluxTask : public SourceTask {

public:

  /**
   * @brief Destructor
   */
  virtual ~IsophotalFluxTask() = default;

  IsophotalFluxTask(SeFloat magnitude_zero_point) : m_magnitude_zero_point(magnitude_zero_point) {}

  virtual void computeProperties(SourceInterface& source) const override;


private:
  SeFloat m_magnitude_zero_point;

}; /* End of IsophotalFluxTask class */


} /* namespace SExtractor */


#endif
