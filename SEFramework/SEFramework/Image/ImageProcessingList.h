/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
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

  virtual std::shared_ptr<Image<T>> processImage(std::shared_ptr<Image<T>> image) const override {
    auto processed_image = image;
    for (auto& processing_step : m_processing_list) {
      processed_image = processing_step->processImage(processed_image);
    }

    return processed_image;
  }
  virtual std::shared_ptr<Image<T>> processImage(std::shared_ptr<Image<T>> image, std::shared_ptr<Image<T>> variance, T threshold) const override {
    auto processed_image = image;
    for (auto& processing_step : m_processing_list) {
      processed_image = processing_step->processImage(processed_image, variance, threshold);
    }

    return processed_image;
  }

private:
  std::vector<std::shared_ptr<ImageProcessing<T>>> m_processing_list;

};

using DetectionImageProcessingList = ImageProcessingList<DetectionImage::PixelType>;

}

#endif /* SEFRAMEWORK_SEFRAMEWORK_IMAGE_IMAGEPROCESSINGLIST_H_ */
