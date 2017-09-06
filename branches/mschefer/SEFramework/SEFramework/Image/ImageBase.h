/*
 * ImageBase.h
 *
 *  Created on: Aug 31, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_IMAGEBASE_H_
#define _SEFRAMEWORK_IMAGE_IMAGEBASE_H_


#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageChunk.h"

namespace SExtractor {

template <typename T>
class ImageBase : public Image<T>, public std::enable_shared_from_this<ImageBase<T>> {

public:
  virtual ~ImageBase() {}

  virtual std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const override {
    return UniversalImageChunk<T>::create(this->shared_from_this(), x, y, width, height);
  }

};

}



#endif /* _SEFRAMEWORK_IMAGE_IMAGEBASE_H_ */
