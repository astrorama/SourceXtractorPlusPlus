/*
 * MoffatCheckImage.h
 *
 *  Created on: 2019 M02 5
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CHECKIMAGES_MOFFATCHECKIMAGE_H_
#define _SEIMPLEMENTATION_CHECKIMAGES_MOFFATCHECKIMAGE_H_

#include "SEUtils/Observable.h"
#include "SEFramework/Image/WriteableImage.h"
#include "SEFramework/Source/SourceGroupInterface.h"

namespace SExtractor {

class MoffatCheckImage : public Observer<std::shared_ptr<SourceGroupInterface>> {
public:

  MoffatCheckImage(std::shared_ptr<WriteableImage<SeFloat>> check_image) :
      m_check_image(check_image) {}

  virtual void handleMessage(const std::shared_ptr<SourceGroupInterface>& group);

private:
  std::shared_ptr<WriteableImage<SeFloat>> m_check_image;
};


} /* namespace SExtractor */



#endif /* _SEIMPLEMENTATION_CHECKIMAGES_MOFFATCHECKIMAGE_H_ */
