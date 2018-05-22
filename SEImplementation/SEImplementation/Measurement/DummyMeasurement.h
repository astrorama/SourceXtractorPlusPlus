/*
 * DummyMeasurement.h
 *
 *  Created on: May 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_MEASUREMENT_DUMMYMEASUREMENT_H_
#define _SEIMPLEMENTATION_MEASUREMENT_DUMMYMEASUREMENT_H_

#include "SEFramework/Pipeline/Measurement.h"

namespace SExtractor {

class DummyMeasurement : public Measurement {
public:

  virtual void handleMessage(const std::shared_ptr<SourceGroupInterface>& source_group) override {
    notifyObservers(source_group);
  }

  virtual void performMeasurements() override {}

};

}


#endif /* SEIMPLEMENTATION_SEIMPLEMENTATION_MEASUREMENT_DUMMYMEASUREMENT_H_ */
