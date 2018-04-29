/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General
 * Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * SourceIDsTaskFactory.h
 *
 *  Created on: Apr 27, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef __SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDSTASKFACTORY_H_
#define __SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDSTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"
#include "SEImplementation/Plugin/SourceIDs/SourceIDsTask.h"

// not working
namespace SExtractor {
class SourceIDsTaskFactory : public TaskFactory {
public:
  SourceIDsTaskFactory() {}
  virtual ~SourceIDsTaskFactory() = default;
  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const {
    //  if (property_id == PropertyId::create<PartitionID>()) {
    return std::make_shared<SourceIDsTask>();
  }
private:
};

}

#endif /* __SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDSTASKFACTORY_H_ */
