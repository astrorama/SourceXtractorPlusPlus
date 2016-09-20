/*
 * ImageProcessing.h
 *
 *  Created on: Sep 15, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_IMAGEPROCESSING_H_
#define _SEFRAMEWORK_IMAGE_IMAGEPROCESSING_H_

#include "SEFramework/Image/Image.h"

namespace SExtractor {

template<typename T>
class ImageProcessing {
public:
  virtual ~ImageProcessing() = default;

  virtual std::shared_ptr<Image<T>> processImage(std::shared_ptr<Image<T>> image) = 0;
};

using DetectionImageProcessing = ImageProcessing<DetectionImage::PixelType>;

}

#endif /* _SEFRAMEWORK_IMAGE_IMAGEPROCESSING_H_ */
