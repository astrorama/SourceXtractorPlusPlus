/** Copyright �� 2019 Universit�� de Gen��ve, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Universit��
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
 * @file CoreThresholdPartitionStep.cpp
 * @date 10/23/19
 * @author mkuemmel
 */

#include "SEImplementation/Plugin/CoreThresholdPartition/CoreThresholdPartitionStep.h"

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Property/PixelCoordinateList.h"

namespace SourceXtractor {

CoreThresholdPartitionStep::CoreThresholdPartitionStep(double snr_level, unsigned int min_pixel_count) :
    m_snr_level(snr_level), m_min_pixel_count(min_pixel_count) {
}

std::vector<std::shared_ptr<SourceInterface>> CoreThresholdPartitionStep::partition(std::shared_ptr<SourceInterface> source) const {
  long int n_snr_level(0);

  // get the SNR image
  const auto& snr_image = source->getProperty<DetectionFrame>().getFrame()->getSnrImage();

  // go over all pixels
  for (auto pixel_coord : source->getProperty<PixelCoordinateList>().getCoordinateList())
    // enhance the counter if the SNR is above the level
    if (snr_image->getValue(pixel_coord.m_x, pixel_coord.m_y) >= m_snr_level)
      n_snr_level += 1;

  // check whether the pixel # is above the threshold
  if (n_snr_level < m_min_pixel_count) {
    return {};
  } else {
    return { source };
  }
}

} // SEImplementation namespace

