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

  ApertureFlagTask(const std::vector<SeFloat> &apertures)
    : m_apertures(apertures) {}

  virtual void computeProperties(SourceInterface &source) const override;

private:
  std::vector<SeFloat> m_apertures;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREFLAGTASK_H_ */
