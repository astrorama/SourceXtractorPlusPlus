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
 * BlendedFlagTask.h
 *
 *  Created on: Oct 29, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAGTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAGTASK_H_


#include "SEFramework/Task/SourceTask.h"
#include "SEImplementation/Property/SourceId.h"
#include "BlendedFlag.h"

namespace SExtractor {

class BlendedFlagTask : public SourceTask {
public:
  virtual ~BlendedFlagTask() = default;

  virtual void computeProperties(SourceInterface &source) const {
    const auto& detection_src_id = source.getProperty<SourceId>();
    source.setProperty<BlendedFlag>(detection_src_id.getDetectionId() != detection_src_id.getSourceId());
  };
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAGTASK_H_
