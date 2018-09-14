/*
 * KronRadiusTaskFactory.h
 *
 *  Created on: Sep 12, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUSTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUSTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class KronRadiusTaskFactory : public TaskFactory {
public:
  KronRadiusTaskFactory() {};

  /// Destructor
  virtual ~KronRadiusTaskFactory() = default;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUSTASKFACTORY_H_ */
