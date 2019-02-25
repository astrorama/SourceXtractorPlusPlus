/*
 * BlendedFlagTask.h
 *
 *  Created on: Oct 29, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAGTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAGTASK_H_


#include "SEFramework/Task/SourceTask.h"
#include "SEImplementation/Property/SourceId.h"
#include "BlendedFlag.h"

namespace SExtractor {

class BlendedFlagTask : public SourceTask {
public:
  virtual ~BlendedFlagTask() = default;

  virtual void computeProperties(SourceInterface &source) const {
    const auto& detection_src_id = source.getProperty<SourceId>();
    source.setProperty<BlendedFlag>(detection_src_id.getDetectionId() != detection_src_id.getSourceId());
  };
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAGTASK_H_
