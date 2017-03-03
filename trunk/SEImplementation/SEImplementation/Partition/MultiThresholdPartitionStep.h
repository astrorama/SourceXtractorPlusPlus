/*
 * MultiThresholdPartitionStep.h
 *
 *  Created on: Jan 17, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PARTITION_MULTITHRESHOLDPARTITIONSTEP_H_
#define _SEIMPLEMENTATION_PARTITION_MULTITHRESHOLDPARTITIONSTEP_H_

#include "SEUtils/Types.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEFramework/Pipeline/Partition.h"
#include "SEFramework/Source/SourceFactory.h"


namespace SExtractor {

class MultiThresholdNode;

/**
 * @class MinAreaPartitionStep
 * @brief A PartitionStep that rejects Sources if they do not have at least a given number of pixels
 *
 */
class MultiThresholdPartitionStep : public PartitionStep {

public:

  MultiThresholdPartitionStep(std::shared_ptr<SourceFactory> source_factory, SeFloat contrast,
      unsigned int thresholds_nb, unsigned int min_deblend_area) :
    m_source_factory(source_factory), m_contrast(contrast), m_thresholds_nb(thresholds_nb), m_min_deblend_area(min_deblend_area) {}

  virtual ~MultiThresholdPartitionStep() = default;

  virtual std::vector<std::shared_ptr<SourceInterface>> partition(std::shared_ptr<SourceInterface> source) const;

private:
  std::vector<std::shared_ptr<SourceInterface>> reassignPixels(
      const std::vector<std::shared_ptr<SourceInterface>>& sources,
      const std::vector<PixelCoordinate>& pixel_coords,
      std::shared_ptr<VectorImage<DetectionImage::PixelType>> image,
      const std::vector<std::shared_ptr<MultiThresholdNode>>& source_nodes
      ) const;

  std::shared_ptr<SourceFactory> m_source_factory;
  SeFloat m_contrast;
  unsigned int m_thresholds_nb;
  unsigned int m_min_deblend_area;
};


}




#endif /* _SEIMPLEMENTATION_PARTITION_MULTITHRESHOLDPARTITIONSTEP_H_ */
