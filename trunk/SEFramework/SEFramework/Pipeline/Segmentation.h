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
 * @brief Segmentation takes an image and splits into individual Sources for further refinement. Each Source
 * results in a notification of the Segmentation's observers.
 *
 */
class Segmentation : public Observable<std::shared_ptr<Source>> {

public:

  /**
   * @brief Destructor
   */
  virtual ~Segmentation() = default;

  virtual void scan(const DetectionImage& image) = 0;

private:

}; /* End of Segmentation class */

} /* namespace SEFramework */

#endif
