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
 * @date 09/01/25
 * @author embray
 */

#ifndef _SEFRAMEWORK_IMAGE_FITSREADER_H
#define _SEFRAMEWORK_IMAGE_FITSREADER_H

#include "SEFramework/Image/BufferedImage.h"
#include "SEFramework/Image/ImageFileReader.h"
#include "SEFramework/FITS/FitsImageSource.h"

namespace SourceXtractor {

/**
 * @class FitsReader
 * @brief
 *
 */
class FitsReader : public ImageFileReader {

public:
  using ImageFileReader::ImageFileReader;

  static bool test(std::istream& stream);
  std::shared_ptr<ImageSource> get() override;
  std::shared_ptr<ImageSource> get(int image_index) override;
  std::shared_ptr<ImageSource> get(const std::string& image_path) override;
  // TODO: Integrate this interface into the ImageFileReader base class
  template <typename T>
  static std::shared_ptr<Image<T>> readFile(const std::string& filename) {
    auto image_source = std::make_shared<FitsImageSource>(filename, 0, ImageTile::getTypeValue(T()));
    return BufferedImage<T>::create(image_source);
  }

}; /* End of FitsReader class */

} /* namespace SourceXtractor */


#endif
