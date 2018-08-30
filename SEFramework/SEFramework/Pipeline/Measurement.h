/*
 * Measurement.h
 *
 *  Created on: May 17, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_PIPELINE_MEASUREMENT_H_
#define _SEFRAMEWORK_PIPELINE_MEASUREMENT_H_


#include "SEUtils/Observable.h"
#include "SEFramework/Source/SourceGroupInterface.h"
#include "SEFramework/Task/TaskProvider.h"

namespace SExtractor {

class Measurement :
    public Observer<std::shared_ptr<SourceGroupInterface>>,
    public Observable<std::shared_ptr<SourceGroupInterface>> {
public:

  virtual ~Measurement() = default;

  virtual void startThreads() = 0;
  virtual void waitForThreads() = 0;
};

}


#endif /* _SEFRAMEWORK_PIPELINE_MEASUREMENT_H_ */
