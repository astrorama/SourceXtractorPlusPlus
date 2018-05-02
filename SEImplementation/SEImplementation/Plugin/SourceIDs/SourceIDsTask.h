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
 * SourceIDsTask.h
 *
 *  Created on: Apr 27, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef __SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDSTASK_H_
#define __SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDSTASK_H_

#include "SourceIDs.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEImplementation/Property/SourceId.h"

namespace SExtractor {
class SourceIDsTask : public SourceTask {
public:
  /// Destructor
  virtual ~SourceIDsTask() = default;

  virtual void computeProperties(SourceInterface& source) const {

     // get the two properties
    const auto& source_id = source.getProperty<SourceId>().getSourceId();
    const auto& detect_id = source.getProperty<SourceId>().getDetectionId();

    // set the source property
    source.setProperty<SourceIDs>(source_id, detect_id);
  }

private:
};
} /* namespace SExtractor */
#endif /* __SEIMPLEMENTATION_PLUGIN_SOURCESIDS_SOURCESIDSTASK_H_ */
