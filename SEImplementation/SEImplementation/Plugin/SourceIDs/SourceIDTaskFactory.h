/*
 * SourceIDTaskFactory.h
 *
 *  Created on: May 30, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDTASKFACTORY_H_


#include "SEFramework/Task/TaskFactory.h"
#include "SEImplementation/Plugin/SourceIDs/SourceIDTask.h"

namespace SExtractor {

class SourceIDTaskFactory : public TaskFactory {
public:
  SourceIDTaskFactory() {}

  /// Destructor
  virtual ~SourceIDTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override {
    if (property_id == PropertyId::create<SourceID>()) {
      return std::make_shared<SourceIDTask>();
    } else {
      return nullptr;
    }
  }
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDTASKFACTORY_H_ */
