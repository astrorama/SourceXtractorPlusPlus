/**
 * @file Segmentation/Lutz/Lutz.h
 * @date 05/31/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATION_LUTZ_H
#define _SEIMPLEMENTATION_SEGMENTATION_LUTZ_H

#include "SEFramework/Source/SourceFactory.h"
#include "SEFramework/Task/TaskProvider.h"
#include "SEFramework/Source/SourceWithOnDemandProperties.h"
#include "SEFramework/Pipeline/Segmentation.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

/**
 * @class Lutz
 * @brief Implements a Segmentation based on the Lutz algorithm
 */
class Lutz : public Segmentation::Labelling {

public:

  /**
   * @brief Destructor
   */
  virtual ~Lutz() = default;

  Lutz(const Segmentation& segmentation, std::shared_ptr<SourceFactory> source_factory)
      : Labelling(segmentation),
        m_source_factory(source_factory) {

    assert(source_factory != nullptr);
  }

  virtual void labelImage(const DetectionImage& image) override;


private:
  class PixelGroup;

  void publishGroup(PixelGroup& pixel_group);

  std::shared_ptr<SourceFactory> m_source_factory;

}; /* End of Lutz class */

} /* namespace SExtractor */


#endif
