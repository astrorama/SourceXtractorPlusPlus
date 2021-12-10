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
 * @file SEImplementation/Partition/AttractorsPartitionStep.h
 * @date 06/03/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PARTITION_ATTRACTORSPARTITIONSTEP_H
#define _SEIMPLEMENTATION_PARTITION_ATTRACTORSPARTITIONSTEP_H

#include <unordered_map>
#include <vector>

#include "SEUtils/PixelCoordinate.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Source/SourceFactory.h"
#include "SEFramework/Source/SourceWithOnDemandProperties.h"
#include "SEFramework/Pipeline/Partition.h"

namespace SourceXtractor {

/**
 * @class AttractorsPartitionStep
 * @brief Splits sources by identifying an attractor pixel by climbing the values gradient from every pixel.
 *
 */
class AttractorsPartitionStep : public PartitionStep {
public:

  /**
   * @brief Destructor
   */
  virtual ~AttractorsPartitionStep() = default;

  explicit AttractorsPartitionStep(std::shared_ptr<SourceFactory> source_factory) :
        m_source_factory(source_factory) {}

  virtual std::vector<std::shared_ptr<SourceInterface>> partition(std::shared_ptr<SourceInterface> source) const override;

private:
  std::shared_ptr<SourceFactory> m_source_factory;

  void attractPixels(
      const std::vector<std::pair<PixelCoordinate, PixelCoordinate>>& pixels_with_origin,
      std::unordered_map<PixelCoordinate, std::vector<PixelCoordinate>>& attractors,
      std::function<DetectionImage::PixelType (PixelCoordinate)> value_function) const;

  std::vector<std::vector<PixelCoordinate>> mergeAttractors(
      const std::unordered_map<PixelCoordinate, std::vector<PixelCoordinate>>& attractors) const;


}; /* End of AttractorsPartitionStep class */

} /* namespace SourceXtractor */


#endif
