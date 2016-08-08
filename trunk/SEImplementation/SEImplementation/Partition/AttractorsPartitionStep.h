/**
 * @file SEImplementation/Partition/AttractorsPartitionStep.h
 * @date 06/03/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PARTITION_ATTRACTORSPARTITIONSTEP_H
#define _SEIMPLEMENTATION_PARTITION_ATTRACTORSPARTITIONSTEP_H

#include <unordered_map>
#include <vector>

#include "SEFramework/Source/SourceFactory.h"
#include "SEFramework/Source/SourceWithOnDemandProperties.h"
#include "SEFramework/Pipeline/Partition.h"

namespace SExtractor {

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

  AttractorsPartitionStep(std::shared_ptr<SourceFactory> source_factory) :
        m_source_factory(source_factory) {}

  virtual std::vector<std::shared_ptr<SourceInterface>> partition(std::shared_ptr<SourceInterface> source) const override;

private:
  std::shared_ptr<SourceFactory> m_source_factory;

  void attractPixels(
      const std::vector<std::pair<PixelCoordinate, PixelCoordinate>>& pixels_with_origin,
      std::unordered_map<PixelCoordinate, std::vector<PixelCoordinate>>& attractors,
      std::function<double (PixelCoordinate)> value_function) const;

  std::vector<std::vector<PixelCoordinate>> mergeAttractors(
      const std::unordered_map<PixelCoordinate, std::vector<PixelCoordinate>>& attractors) const;


}; /* End of AttractorsPartitionStep class */

} /* namespace SExtractor */


#endif
