/*
 * BlendedFlagTaskFactory.h
 *
 *  Created on: Oct 29, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAGTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAGTASKFACTORY_H_


#include "SEFramework/Task/TaskFactory.h"
#include "SEImplementation/Plugin/BlendedFlag/BlendedFlagTask.h"
#include "SEImplementation/Plugin/BlendedFlag/BlendedFlag.h"

namespace SExtractor {

class BlendedFlagTaskFactory : public TaskFactory {
public:
  BlendedFlagTaskFactory() {}

  virtual ~BlendedFlagTaskFactory() = default;

  virtual std::shared_ptr<Task> createTask(const PropertyId &property_id) const {
    if (property_id == PropertyId::create<BlendedFlag>()) {
      return std::make_shared<BlendedFlagTask>();
    }
    return nullptr;
  }
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAGTASKFACTORY_H_
