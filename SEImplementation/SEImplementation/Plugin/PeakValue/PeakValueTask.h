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

namespace SExtractor {

class PeakValueTask : public SourceTask {

public:

  virtual ~PeakValueTask() = default;

  virtual void computeProperties(SourceInterface& source) const override {
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


} /* namespace SExtractor */


#endif /* _SEIMPLEMENTATION_PLUGIN_PEAKVALUE_PEAKVALUETASK_H_ */
