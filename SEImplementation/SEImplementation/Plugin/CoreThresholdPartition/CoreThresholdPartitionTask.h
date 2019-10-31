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
/**
 * @file CoreThresholdPartitionTask.h
 *
 * @date May 27, 2019
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_CORETHRESHOLDTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_CORETHRESHOLDTASK_H_

#include "NCorePixel.h"
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Property/PixelCoordinateList.h"

#include "SEFramework/Task/SourceTask.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"

#include "SEImplementation/Measurement/MultithreadedMeasurement.h"

namespace SourceXtractor {

class CoreThresholdPartitionTask : public SourceTask {

public:
  CoreThresholdPartitionTask(double snr_level): m_snr_level(snr_level) {};

  virtual ~CoreThresholdPartitionTask() = default;

  virtual void computeProperties(SourceInterface& source) const {
    std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

    long int n_snr_level=0;

    // get the detection frame and the SNR image
    const auto& snr_image    = source.getProperty<DetectionFrame>().getFrame()->getSnrImage();

    // go over all pixels
    for (auto pixel_coord : source.getProperty<PixelCoordinateList>().getCoordinateList())
      // enhance the counter if the SNR is above the level
      if (snr_image->getValue(pixel_coord.m_x, pixel_coord.m_y) >= m_snr_level)
	n_snr_level += 1;

    // set the property
    source.setProperty<NCorePixel>(n_snr_level);
};

private:
  double m_snr_level;

}; // End of CoreThresholdTask class
} // namespace SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_SNRLEVELSOURCETASK_H_ */



