/*
 * SourceIDsTask.h
 *
 *  Created on: Apr 27, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef __SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDSTASK_H_
#define __SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDSTASK_H_

#include "SEUtils/PixelCoordinate.h"
#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class SourceIDsTask : public SourceTask {
public:

  /// Destructor
  virtual ~SourceIDsTask() = default;

  // Constructor
  SourceIDsTask() {}

  virtual void computeProperties(SourceInterface& source){;
    // there should be the code
  source.setProperty<SourceIDs>(0);
  }

private:
};

}

#endif /* __SEIMPLEMENTATION_PLUGIN_SOURCESIDS_SOURCESIDSTASK_H_ */
