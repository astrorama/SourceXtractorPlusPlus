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
 * @file SEImplementation/Partition/MinAreaPartitionStep.h
 * @date 05/30/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PARTITION_MINAREAPARTITIONSTEP_H
#define _SEIMPLEMENTATION_PARTITION_MINAREAPARTITIONSTEP_H

#include "SEFramework/Pipeline/Partition.h"

namespace SExtractor {

/**
 * @class MinAreaPartitionStep
 * @brief A PartitionStep that rejects Sources if they do not have at least a given number of pixels
 *
 */
class MinAreaPartitionStep : public PartitionStep {

public:

  /// Destructor
  virtual ~MinAreaPartitionStep() = default;

  /// Constructor
  MinAreaPartitionStep(unsigned int min_pixel_count);

  virtual std::vector<std::shared_ptr<SourceInterface>> partition(std::shared_ptr<SourceInterface> source) const override;


private:
  unsigned int m_min_pixel_count;


}; /* End of MinAreaPartitionStep class */

} /* namespace SExtractor */


#endif
