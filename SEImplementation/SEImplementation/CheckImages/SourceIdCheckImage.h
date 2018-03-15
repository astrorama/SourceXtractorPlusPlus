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
      m_check_image(check_image),
      m_object_id(1)
  {}

  virtual void handleMessage(const std::shared_ptr<SourceInterface>& source);

private:
  std::shared_ptr<WriteableImage<unsigned int>> m_check_image;
  unsigned int m_object_id;

};

}


#endif /* SEIMPLEMENTATION_SEIMPLEMENTATION_CHECKIMAGES_SEGMENTATIONCHECKIMAGE_H_ */
