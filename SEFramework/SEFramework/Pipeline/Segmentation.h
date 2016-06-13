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

namespace SExtractor {

class Image;

/**
 * @class Segmentation
 * @brief
 *
 */
class Segmentation : public Observable<std::shared_ptr<Source>> {

public:

  /**
   * @brief Destructor
   */
  virtual ~Segmentation() = default;

  virtual void scan(const Image& image) = 0;

private:

}; /* End of Segmentation class */

} /* namespace SEFramework */

#endif
