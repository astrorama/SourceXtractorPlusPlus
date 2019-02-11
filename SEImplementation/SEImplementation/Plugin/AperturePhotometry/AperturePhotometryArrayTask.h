/*
 * AperturePhotometryArrayTask.h
 *
 *  Created on: Nov 23, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYARRAYTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYARRAYTASK_H_

#include "SEFramework/Aperture/Aperture.h"
#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class AperturePhotometryArrayTask : public SourceTask {
public:

  virtual ~AperturePhotometryArrayTask() = default;

  AperturePhotometryArrayTask(const std::vector<unsigned> &aperture_instances, unsigned instance)
    : m_aperture_instances(aperture_instances), m_instance(instance) {}

  virtual void computeProperties(SourceInterface &source) const override;

private:
  std::vector<unsigned> m_aperture_instances;
  unsigned m_instance;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYARRAYTASK_H_
