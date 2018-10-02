/*
 * MeasurementNeighbourInfoTask.h
 *
 *  Created on: Sep 28, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTNEIGHBOURINFO_MEASUREMENTNEIGHBOURINFOTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTNEIGHBOURINFO_MEASUREMENTNEIGHBOURINFOTASK_H_

#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class MeasurementNeighbourInfoTask: public SourceTask {
public:
  virtual ~MeasurementNeighbourInfoTask() = default;

  MeasurementNeighbourInfoTask(unsigned instance);

  void computeProperties(SourceInterface& source) const override;

private:
  unsigned m_instance;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_MEASUREMENTNEIGHBOURINFO_MEASUREMENTNEIGHBOURINFOTASK_H_
