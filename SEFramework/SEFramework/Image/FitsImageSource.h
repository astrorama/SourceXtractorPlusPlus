/*
 * FitsImageSource.h
 *
 *  Created on: Feb 21, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_FITSIMAGESOURCE_H_
#define _SEFRAMEWORK_IMAGE_FITSIMAGESOURCE_H_

#include <CCfits/CCfits>

#include "SEFramework/Image/ImageSource.h"

namespace SExtractor {

template <typename T>
class FitsImageSource : public ImageSource<T> {
public:

  FitsImageSource(const std::string& filename) : m_width(500), m_height(500) {}  //FIXME

  virtual std::shared_ptr<ImageTile<T>> getImageTile(int x, int y, int width, int height) const override {
    return std::make_shared<ImageTile<T>>(x, y, width, height);
  }

  /// Returns the width of the image in pixels
  virtual int getWidth() const override {
    return m_width;
  }

  /// Returns the height of the image in pixels
  virtual int getHeight() const  override {
    return m_height;
  }

private:
  int m_width;
  int m_height;
};

}




#endif /* _SEFRAMEWORK_IMAGE_FITSIMAGESOURCE_H_ */
