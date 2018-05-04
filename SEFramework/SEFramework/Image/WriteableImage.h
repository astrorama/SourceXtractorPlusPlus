/*
 * WriteableImage.h
 *
 *  Created on: Mar 8, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_WRITEABLEIMAGE_H_
#define _SEFRAMEWORK_IMAGE_WRITEABLEIMAGE_H_

#include "SEFramework/Image/Image.h"

namespace SExtractor {

template <typename T>
class WriteableImage : public virtual Image<T> {
public:

  virtual void setValue(int x, int y, T value) = 0;
  //virtual void setValues(int x, int y, int width, int height, T* values) = 0;
};

}

#endif /* _SEFRAMEWORK_IMAGE_WRITEABLEIMAGE_H_ */
