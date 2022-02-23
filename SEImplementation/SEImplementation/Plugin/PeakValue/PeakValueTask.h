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
 * PeakValueTask.h
 *
 *  Created on: Feb 9, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_PEAKVALUE_PEAKVALUETASK_H_
#define _SEIMPLEMENTATION_PLUGIN_PEAKVALUE_PEAKVALUETASK_H_

#include "SEFramework/Task/SourceTask.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"

namespace SourceXtractor {

class PeakValueTask : public SourceTask {

public:

  virtual ~PeakValueTask() = default;

  void computeProperties(SourceInterface& source) const override {
    // FIXME is it correct to use filtered values?
    const auto& pixel_values = source.getProperty<DetectionFramePixelValues>().getFilteredValues();

    DetectionImage::PixelType peak_value = std::numeric_limits<DetectionImage::PixelType>::min();
    DetectionImage::PixelType min_value = std::numeric_limits<DetectionImage::PixelType>::max();
    for (auto value : pixel_values) {
      peak_value = std::max(peak_value, value);
      min_value = std::min(min_value, value);
    }

    source.setProperty<PeakValue>(min_value, peak_value);
  }


private:

};


} /* namespace SourceXtractor */


#endif /* _SEIMPLEMENTATION_PLUGIN_PEAKVALUE_PEAKVALUETASK_H_ */
