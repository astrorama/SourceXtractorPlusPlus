/*
 * ShapeParametersTaskFactory.h
 *
 *  Created on: Jan 27, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SHAPEPARAMETERS_SHAPEPARAMETERSTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_SHAPEPARAMETERS_SHAPEPARAMETERSTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class ShapeParametersTaskFactory : public TaskFactory {
public:
  ShapeParametersTaskFactory() {}

  /// Destructor
  virtual ~ShapeParametersTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;
};


} /* namespace SExtractor */



#endif /* _SEIMPLEMENTATION_PLUGIN_SHAPEPARAMETERS_SHAPEPARAMETERSTASKFACTORY_H_ */
