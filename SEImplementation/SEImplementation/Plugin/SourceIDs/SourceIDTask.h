/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*
 * SourceIDTask.h
 *
 *  Created on: May 30, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDTASK_H_

#include <atomic>
#include "SEFramework/Task/SourceTask.h"
#include "SEImplementation/Property/SourceId.h"
#include "SEImplementation/Plugin/SourceIDs/SourceID.h"

namespace SourceXtractor {

class SourceIDTask : public SourceTask {

public:
  virtual ~SourceIDTask() = default;

  SourceIDTask() {}

  virtual void computeProperties(SourceInterface& source) const override {
    auto detection_id = source.getProperty<SourceId>().getDetectionId();
    source.setProperty<SourceID>(getNewId(), detection_id);
  }

private:
  static unsigned int getNewId() {
    static std::atomic<std::uint32_t> s_id(1);
    return s_id++;
  }

};

} /* namespace SourceXtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDTASK_H_ */
