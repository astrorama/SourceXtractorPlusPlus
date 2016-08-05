/**
 * @file SEFramework/Pipeline/Segmentation.h
 * @date 05/27/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_PIPELINE_SEGMENTATION_H
#define _SEFRAMEWORK_PIPELINE_SEGMENTATION_H

#include <memory>

#include "SEUtils/Observable.h"
#include "SEFramework/Source/Source.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

/**
 * @class Segmentation
 * @brief Segmentation takes an image and splits it into individual Sources for further refinement. Each Source
 * results in a notification of the Segmentation's Observers.
 *
 */
class Segmentation : public Observable<std::shared_ptr<Source>> {

public:

  /// Destructor
  virtual ~Segmentation() = default;

  /// Processes a DetectionImage notifying Observers with a Source object for each detection
  virtual void scan(const DetectionImage& image) = 0;

private:

}; /* End of Segmentation class */

} /* namespace SExtractor */

#endif
