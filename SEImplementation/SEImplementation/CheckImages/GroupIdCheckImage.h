/*
 * GroupIdCheckImage.h
 *
 *  Created on: 2019 M01 30
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CHECKIMAGES_GROUPIDCHECKIMAGE_H_
#define _SEIMPLEMENTATION_CHECKIMAGES_GROUPIDCHECKIMAGE_H_

#include "SEUtils/Observable.h"
#include "SEFramework/Image/WriteableImage.h"
#include "SEFramework/Source/SourceGroupInterface.h"

namespace SExtractor {

class GroupIdCheckImage : public Observer<std::shared_ptr<SourceGroupInterface>> {
public:

  GroupIdCheckImage(std::shared_ptr<WriteableImage<unsigned int>> check_image) :
      m_check_image(check_image) {}

  virtual void handleMessage(const std::shared_ptr<SourceGroupInterface>& group);

private:
  std::shared_ptr<WriteableImage<unsigned int>> m_check_image;
};


} /* namespace SExtractor */


#endif /* SEIMPLEMENTATION_SEIMPLEMENTATION_CHECKIMAGES_GROUPIDCHECKIMAGE_H_ */
