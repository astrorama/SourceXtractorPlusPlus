/*
 * DetectionIdCheckImage.h
 *
 *  Created on: Jun 25, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CHECKIMAGES_DETECTIONIDCHECKIMAGE_H_
#define _SEIMPLEMENTATION_CHECKIMAGES_DETECTIONIDCHECKIMAGE_H_

#include "SEUtils/Observable.h"
#include "SEFramework/Image/WriteableImage.h"
#include "SEFramework/Source/SourceInterface.h"

namespace SExtractor {

class DetectionIdCheckImage : public Observer<std::shared_ptr<SourceInterface>> {
public:

  DetectionIdCheckImage(std::shared_ptr<WriteableImage<unsigned int>> check_image) :
      m_check_image(check_image)
      {}

  virtual void handleMessage(const std::shared_ptr<SourceInterface>& source);

private:
  std::shared_ptr<WriteableImage<unsigned int>> m_check_image;
};

}

#endif /* _SEIMPLEMENTATION_CHECKIMAGES_DETECTIONIDCHECKIMAGE_H_ */
