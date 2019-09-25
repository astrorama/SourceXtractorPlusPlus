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
 * @file Overall/Pipeline/PixelRefinement.h
 * @date 05/23/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_PIPELINE_PARTITION_H
#define _SEFRAMEWORK_PIPELINE_PARTITION_H

#include "SEUtils/Observable.h"
#include "SEFramework/Source/SourceInterface.h"

namespace SExtractor {

/**
 * @class PartitionStep
 * @brief A PartitionStep gets applied on a single Source and can result any number of Sources being outputed.
 *  (including the same one, none, or any number of new Sources)
 *
 */
class PartitionStep {
public:

  /**
   * @brief Destructor
   */
  virtual ~PartitionStep() = default;

  virtual std::vector<std::shared_ptr<SourceInterface>> partition(std::shared_ptr<SourceInterface> source) const = 0;
};

/**
 * @class Partition
 * @brief For each Source it receives, applies PartitionSteps and outputs one or more Sources.
 *
 * @details The first PartitionStep is applied to the Source that was received. Each subsequent PartitionStep
 * is applied to the Source(s) produced by the previous step. The Sources resulting from the last step are
 * notified to the Observers one by one.
 *
 */
class Partition : public Observer<std::shared_ptr<SourceInterface>>, public Observable<std::shared_ptr<SourceInterface>> {

public:

  /**
   * @brief Destructor
   */
  virtual ~Partition() = default;

  /// Constructor - takes a vector of PartitionSteps to be applied in order
  Partition(std::vector<std::shared_ptr<PartitionStep>> steps);

  /// Handles a Source (applies PartitionSteps) and notifies the Observers for every Source in the final result
  virtual void handleMessage(const std::shared_ptr<SourceInterface>& source) override;

private:
  std::vector<std::shared_ptr<PartitionStep>> m_steps;

}; /* End of Partition class */

} /* namespace SExtractor */


#endif
