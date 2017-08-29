/*
 * PeakValueTaskFactory.h
 *
 *  Created on: Feb 9, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_PEAKVALUE_PEAKVALUETASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_PEAKVALUE_PEAKVALUETASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"

#include "SEImplementation/Plugin/PeakValue/PeakValue.h"
#include "SEImplementation/Plugin/PeakValue/PeakValueTask.h"


namespace SExtractor {

class PeakValueTaskFactory : public TaskFactory {
public:
  PeakValueTaskFactory() {}

  /// Destructor
  virtual ~PeakValueTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override {
    if (property_id == PropertyId::create<PeakValue>()) {
      return std::make_shared<PeakValueTask>();
    } else {
      return nullptr;
    }
  }
};


} /* namespace SExtractor */


#endif /* _SEIMPLEMENTATION_PLUGIN_PEAKVALUE_PEAKVALUETASKFACTORY_H_ */
