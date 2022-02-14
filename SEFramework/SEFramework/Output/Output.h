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
 * Output.h
 *
 *  Created on: Jun 23, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_PIPELINE_OUTPUT_H_
#define _SEFRAMEWORK_PIPELINE_OUTPUT_H_

#include "SEUtils/Observable.h"
#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/Source/SourceGroupInterface.h"

namespace SourceXtractor {

class Output :
    public Observer<std::shared_ptr<SourceInterface>>,
    public Observer<std::shared_ptr<SourceGroupInterface>> {

public:

  virtual ~Output() = default;

  virtual void handleMessage(const std::shared_ptr<SourceInterface>& source) override {
    outputSource(*source);
  }

  virtual void handleMessage(const std::shared_ptr<SourceGroupInterface>& source_group) override {
    for (auto& source : *source_group) {
      outputSource(source);
    }
  }

  virtual void outputSource(const SourceInterface& source) = 0;

  /// @return Number of elements written
  virtual size_t flush() = 0;

  virtual void nextPart() = 0;
};

}

#endif /* _SEFRAMEWORK_PIPELINE_OUTPUT_H_ */
