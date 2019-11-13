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
 * ImageSource.h
 *
 *  Created on: Feb 14, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_IMAGESOURCE_H_
#define _SEFRAMEWORK_IMAGE_IMAGESOURCE_H_

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageTile.h"

namespace SExtractor {

class ImageSourceBase {
public:
  virtual ~ImageSourceBase() = default;

  /// Human readable representation of this source
  virtual std::string getRepr() const = 0;
};

template <typename T>
class ImageSource : public ImageSourceBase {
public:

  ImageSource() {}

  virtual ~ImageSource() = default;

  virtual std::shared_ptr<ImageTile<T>> getImageTile(int x, int y, int width, int height) const = 0;

  virtual void saveTile(ImageTile<T>& tile) = 0;

  /// Returns the width of the image in pixels
  virtual int getWidth() const = 0;

  /// Returns the height of the image in pixels
  virtual int getHeight() const = 0;

private:

};

}

#endif /* _SEFRAMEWORK_IMAGE_IMAGESOURCE_H_ */
