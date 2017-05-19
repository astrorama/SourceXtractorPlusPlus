/*
 * ImageInterfaceTraits.h
 *
 *  Created on: May 3, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_IMAGE_IMAGEINTERFACETRAITS_H_
#define _SEIMPLEMENTATION_IMAGE_IMAGEINTERFACETRAITS_H_

#include <memory>
#include <utility>


#include "ModelFitting/ModelFitting/Image/ImageTraits.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"

namespace ModelFitting {

using ImageInterfaceType = SExtractor::VectorImage<SExtractor::SeFloat>;
using ImageInterfaceTypePtr = std::shared_ptr<ImageInterfaceType>;

template <>
struct ImageTraits<ImageInterfaceTypePtr> {

  //using iterator = decltype(std::declval<cv::Mat>().begin<double>());
  using iterator = std::vector<ImageInterfaceType::PixelType>::iterator;

  static ImageInterfaceTypePtr factory(std::size_t width, std::size_t height) {
    return std::make_shared<SExtractor::VectorImage<ImageInterfaceType::PixelType>>(width, height);
  }

  static std::size_t width(const ImageInterfaceTypePtr& image) {
    return image->getWidth();
  }

  static std::size_t height(const ImageInterfaceTypePtr& image) {
    return image->getHeight();
  }

  static ImageInterfaceType::PixelType& at(ImageInterfaceTypePtr& image, std::size_t x, std::size_t y) {
    return image->at(x, y);
  }

  static iterator begin(const ImageInterfaceTypePtr& image) {
    return image->getData().begin();
  }

  static iterator end(const ImageInterfaceTypePtr& image) {
    return image->getData().end();
  }

  static void addImageToImage(ImageInterfaceTypePtr& image1, const ImageInterfaceTypePtr& image2,
                              double scale_factor, double x, double y) {
  }

}; // end of class ImageTraits<ImageInterfaceTypePtr>

} // end of namespace ModelFitting


#endif /* _SEIMPLEMENTATION_IMAGE_IMAGEINTERFACETRAITS_H_ */
