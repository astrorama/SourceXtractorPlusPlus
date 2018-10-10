/*
 * ApertureFlagTask.h
 *
 *  Created on: Oct 09, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREFLAGTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREFLAGTASK_H_

#include "SEFramework/Aperture/Aperture.h"
#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class ApertureFlagTask : public SourceTask {
public:

  virtual ~ApertureFlagTask() = default;

  ApertureFlagTask(std::shared_ptr<Aperture> aperture, unsigned int instance)
    : m_aperture(aperture), m_instance(instance) {}

  virtual void computeProperties(SourceInterface &source) const override;

private:
  std::shared_ptr<Aperture> m_aperture;
  unsigned int m_instance;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREFLAGTASK_H_ */
