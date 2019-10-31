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
 * ImageBase.h
 *
 *  Created on: Aug 31, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_IMAGEBASE_H_
#define _SEFRAMEWORK_IMAGE_IMAGEBASE_H_


#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/WriteableImage.h"
#include "SEFramework/Image/ImageChunk.h"

namespace SourceXtractor {

template <typename T>
class ImageBase : public virtual Image<T>, public std::enable_shared_from_this<ImageBase<T>> {

public:
  virtual ~ImageBase() {}

  virtual std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const override {
    return UniversalImageChunk<T>::create(this->shared_from_this(), x, y, width, height);
  }

};

}



#endif /* _SEFRAMEWORK_IMAGE_IMAGEBASE_H_ */
