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
 * ProcessingImageSource.h
 *
 *  Created on: May 2, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_PROCESSINGIMAGESOURCE_H_
#define _SEFRAMEWORK_IMAGE_PROCESSINGIMAGESOURCE_H_

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageSource.h"

namespace SourceXtractor {

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

  virtual void saveTile(ImageTile<T>& /*tile*/) override {
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

  std::string getImageRepr() const {
    return m_image->getRepr();
  }

private:
  std::shared_ptr<Image<T>> m_image;
};

}



#endif /* _SEFRAMEWORK_IMAGE_PROCESSINGIMAGESOURCE_H_ */
