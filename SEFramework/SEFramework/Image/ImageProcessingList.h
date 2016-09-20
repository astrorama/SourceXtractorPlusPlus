/*
 * ImageProcessingList.h
 *
 *  Created on: Sep 15, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_IMAGEPROCESSINGLIST_H_
#define _SEFRAMEWORK_IMAGE_IMAGEPROCESSINGLIST_H_

#include <memory>
#include <vector>

#include "SEFramework/Image/ImageProcessing.h"

namespace SExtractor {

template<typename T>
class ImageProcessingList : public ImageProcessing<T> {
public:
  virtual ~ImageProcessingList() = default;

  explicit ImageProcessingList(const std::vector<std::shared_ptr<ImageProcessing<T>>>& processing_list)
      : m_processing_list(processing_list) {}

  virtual std::shared_ptr<Image<T>> processImage(std::shared_ptr<Image<T>> image) {
    auto processed_image = image;
    for (auto& processing_step : m_processing_list) {
      processed_image = processing_step->processImage(processed_image);
    }

    return processed_image;
  }

private:
  std::vector<std::shared_ptr<ImageProcessing<T>>> m_processing_list;

};

using DetectionImageProcessingList = ImageProcessingList<DetectionImage::PixelType>;

}

#endif /* SEFRAMEWORK_SEFRAMEWORK_IMAGE_IMAGEPROCESSINGLIST_H_ */
