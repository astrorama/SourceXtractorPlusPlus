/*
 * ShapeParametersTask.h
 *
 *  Created on: Jan 27, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SHAPEPARAMETERS_SHAPEPARAMETERSTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_SHAPEPARAMETERS_SHAPEPARAMETERSTASK_H_

#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class ShapeParametersTask : public SourceTask {

public:

  virtual ~ShapeParametersTask() = default;

  virtual void computeProperties(SourceInterface& source) const override;

private:

};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_SHAPEPARAMETERS_SHAPEPARAMETERSTASK_H_ */
