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

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/Image/ImageTile.h"

namespace SourceXtractor {

std::shared_ptr<ImageTile> ImageTile::create(ImageType image_type, int x, int y, int width, int height, std::shared_ptr<ImageSource> source) {
  switch (image_type) {
  default:
  case FloatImage:
    return std::make_shared<ImageTileWithType<float>>(x, y, width, height, source);
  case DoubleImage:
    return std::make_shared<ImageTileWithType<double>>(x, y, width, height, source);
  case IntImage:
    return std::make_shared<ImageTileWithType<int>>(x, y, width, height, source);
  case UIntImage:
    return std::make_shared<ImageTileWithType<unsigned int>>(x, y, width, height, source);
  case LongLongImage:
    return std::make_shared<ImageTileWithType<std::int64_t>>(x, y, width, height, source);
  }
}

void ImageTile::saveIfModified() {
  if (isModified()) {
    m_source->saveTile(*this);
    setModified(false);
  }
}

}


