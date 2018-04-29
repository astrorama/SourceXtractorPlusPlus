/*
 * SegmentationCheckImage.h
 *
 *  Created on: Dec 11, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CHECKIMAGES_SEGMENTATIONCHECKIMAGE_H_
#define _SEIMPLEMENTATION_CHECKIMAGES_SEGMENTATIONCHECKIMAGE_H_

#include "SEUtils/Observable.h"
#include "SEFramework/Image/WriteableImage.h"
#include "SEFramework/Source/SourceInterface.h"

namespace SExtractor {

class SourceIdCheckImage : public Observer<std::shared_ptr<SourceInterface>> {
public:

  SourceIdCheckImage(std::shared_ptr<WriteableImage<unsigned int>> check_image) :
      m_check_image(check_image)
      {}

  virtual void handleMessage(const std::shared_ptr<SourceInterface>& source);

private:
  std::shared_ptr<WriteableImage<unsigned int>> m_check_image;
};
} /* namespace SExtractor */
#endif /* SEIMPLEMENTATION_SEIMPLEMENTATION_CHECKIMAGES_SEGMENTATIONCHECKIMAGE_H_ */
