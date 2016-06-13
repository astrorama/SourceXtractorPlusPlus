/**
 * @file SEImplementation/Partition/AttractorsPartitionStep.h
 * @date 06/03/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PARTITION_ATTRACTORSPARTITIONSTEP_H
#define _SEIMPLEMENTATION_PARTITION_ATTRACTORSPARTITIONSTEP_H

#include <unordered_map>
#include <vector>

#include "SEFramework/Source/Source.h"
#include "SEFramework/Pipeline/Partition.h"

namespace SExtractor {

/**
 * @class AttractorsPartitionStep
 * @brief
 *
 */
class AttractorsPartitionStep : public PartitionStep {
public:

  /**
   * @brief Destructor
   */
  virtual ~AttractorsPartitionStep() = default;

  AttractorsPartitionStep(std::shared_ptr<TaskRegistry> task_registry) : m_task_registry(task_registry) {}

  virtual std::vector<std::shared_ptr<Source>> partition(
      std::shared_ptr<Source> source) const override;

private:
  std::shared_ptr<TaskRegistry> m_task_registry;

  void attractPixels(
      const std::vector<std::pair<PixelCoordinate, PixelCoordinate>>& pixels_with_origin,
      std::unordered_map<PixelCoordinate, std::vector<PixelCoordinate>>& attractors,
      std::function<double (PixelCoordinate)> value_function) const;

  std::vector<std::vector<PixelCoordinate>> mergeAttractors(
      const std::unordered_map<PixelCoordinate, std::vector<PixelCoordinate>>& attractors) const;


}; /* End of AttractorsPartitionStep class */

} /* namespace SEImplementation */


#endif
