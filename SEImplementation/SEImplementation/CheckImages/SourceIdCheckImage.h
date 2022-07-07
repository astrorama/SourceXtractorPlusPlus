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
 * SegmentationCheckImage.h
 *
 *  Created on: Dec 11, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CHECKIMAGES_SEGMENTATIONCHECKIMAGE_H_
#define _SEIMPLEMENTATION_CHECKIMAGES_SEGMENTATIONCHECKIMAGE_H_

#include "SEUtils/Observable.h"
#include "SEFramework/Image/WriteableImage.h"
#include "SEFramework/Source/SourceGroupInterface.h"

namespace SourceXtractor {

class SourceIdCheckImage : public Observer<SourceGroupInterface> {
public:

  SourceIdCheckImage() = default;

  void handleMessage(const SourceGroupInterface& group) override;
};


} /* namespace SourceXtractor */

#endif /* SEIMPLEMENTATION_SEIMPLEMENTATION_CHECKIMAGES_SEGMENTATIONCHECKIMAGE_H_ */
