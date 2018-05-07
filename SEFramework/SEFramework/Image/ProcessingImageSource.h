/*
 * ProcessingImageSource.h
 *
 *  Created on: May 2, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_PROCESSINGIMAGESOURCE_H_
#define _SEFRAMEWORK_IMAGE_PROCESSINGIMAGESOURCE_H_

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageSource.h"

namespace SExtractor {

template <typename T>
class ProcessingImageSource : public ImageSource<T>, public std::enable_shared_from_this<ImageSource<T>>  {
public:

  ProcessingImageSource(std::shared_ptr<Image<T>> image) : m_image(image) {}

  virtual ~ProcessingImageSource() = default;

  virtual std::shared_ptr<ImageTile<T>> getImageTile(int x, int y, int width, int height) const override {
    auto tile = std::make_shared<ImageTile<T>>((const_cast<ProcessingImageSource*>(this))->shared_from_this(), x, y, width, height);

    generateTile(m_image, *tile, x, y, width, height);

    return tile;
  }

  virtual void saveTile(ImageTile<T>& tile) override {
    assert(false);
  }

  /// Returns the width of the image in pixels
  virtual int getWidth() const override {
    return m_image->getWidth();
  }

  /// Returns the height of the image in pixels
  virtual int getHeight() const override {
    return m_image->getHeight();
  }

protected:
  virtual void generateTile(std::shared_ptr<Image<T>> image, ImageTile<T>& tile, int x, int y, int width, int height) const = 0;

private:
  std::shared_ptr<Image<T>> m_image;
};

}



#endif /* _SEFRAMEWORK_IMAGE_PROCESSINGIMAGESOURCE_H_ */
