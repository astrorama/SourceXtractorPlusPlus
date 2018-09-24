/*
 * AperturePhotometryTask.h
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUSTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUSTASK_H_

//#include <memory>

#include "SEUtils/PixelCoordinate.h"
#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class KronRadiusTask : public SourceTask {
public:

  /// Destructor
  virtual ~KronRadiusTask() = default;

  KronRadiusTask() {}

  virtual void computeProperties(SourceInterface& source) const override;

};
}

#endif /* _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUSTASK_H_ */
