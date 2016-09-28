/*
 * AperturePhotometryTask.h
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASK_H_

#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class AperturePhotometryTask : public SourceTask {
public:

  /**
   * @brief Destructor
   */
  virtual ~AperturePhotometryTask() = default;

  AperturePhotometryTask(SeFloat radius, unsigned int instance) : m_radius(radius), m_instance(instance) {}

  virtual void computeProperties(SourceInterface& source) const override;

private:
  SeFloat m_radius;
  unsigned int m_instance;

};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASK_H_ */
