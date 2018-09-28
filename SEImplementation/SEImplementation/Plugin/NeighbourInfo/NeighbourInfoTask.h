/*
 * NeighbourInfoTask.h
 *
 *  Created on: Sep 28, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_NEIGHBOURINFO_NEIGHBOURINFOTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_NEIGHBOURINFO_NEIGHBOURINFOTASK_H_

#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class NeighbourInfoTask: public SourceTask {
public:
  virtual ~NeighbourInfoTask() = default;

  NeighbourInfoTask(unsigned instance);

  void computeProperties(SourceInterface& source) const override;

private:
  unsigned m_instance;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_NEIGHBOURINFO_NEIGHBOURINFOTASK_H_
