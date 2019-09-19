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
/**
 * @file SEFramework/Image/FitsReader.h
 * @date 06/14/16
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_IMAGE_FITSREADER_H
#define _SEFRAMEWORK_IMAGE_FITSREADER_H

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/BufferedImage.h"
#include "SEFramework/FITS/FitsImageSource.h"

namespace SExtractor {

/**
 * @class FitsReader
 * @brief
 *
 */
template <typename T>
class FitsReader {

public:

  /**
   * @brief Destructor
   */
  virtual ~FitsReader() = default;

  static std::shared_ptr<Image<T>> readFile(const std::string& filename) {
    auto image_source = std::make_shared<FitsImageSource<T>>(filename);
    return BufferedImage<T>::create(image_source);
  }

}; /* End of FitsReader class */

} /* namespace SExtractor */


#endif
