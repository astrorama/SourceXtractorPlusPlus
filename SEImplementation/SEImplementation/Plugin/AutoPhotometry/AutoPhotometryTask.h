/*
 * AperturePhotometryTask.h
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYTASK_H_

#include "SEUtils/Types.h"
#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class AutoPhotometryTask : public SourceTask {
public:

  /// Destructor
  virtual ~AutoPhotometryTask() = default;

  AutoPhotometryTask(unsigned instance, SeFloat magnitude_zero_point, SeFloat kron_factor, SeFloat kron_minrad, bool use_symmetry) :
    m_instance(instance),
    m_magnitude_zero_point(magnitude_zero_point),
    m_kron_factor(kron_factor),
    m_kron_minrad(kron_minrad),
    m_use_symmetry(use_symmetry) {}

  virtual void computeProperties(SourceInterface& source) const override;

private:
  unsigned m_instance;
  SeFloat m_magnitude_zero_point;
  SeFloat m_kron_factor;
  SeFloat m_kron_minrad;
  bool m_use_symmetry;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYTASK_H_ */
