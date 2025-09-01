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
 * @file SEFramework/Image/FitsReader.cpp
 * @date 09/01/25
 * @author embray
 */

#include <ElementsKernel/Exception.h>

#include "SEFramework/FITS/FitsReader.h"
#include "SEFramework/Image/ImageFileReader.h"
#include "SEFramework/Image/ImageSource.h"


namespace SourceXtractor {


bool FitsReader::test(std::istream& stream) {
    char header[9] = {0};
    if (!stream.read(header, 9)) {
        return false;
    }

    return std::strncmp(header, "SIMPLE  =", 9) == 0;
}


std::shared_ptr<ImageSource> FitsReader::get() {
  if (0 == m_image_index) {
    // If the primary HDU was not an image try the first extension HDU
    try {
      return get(1);
    } catch (...) {
      return get(2);
    }
  }

  return ImageFileReader::get();
}


std::shared_ptr<ImageSource> FitsReader::get(int hdu_num) {
  return std::make_shared<FitsImageSource>(m_filename, hdu_num);
}


std::shared_ptr<ImageSource> FitsReader::get(const std::string& extname) {
  return std::make_shared<FitsImageSource>(m_filename, extname);
}


static ImageFileReader::StaticFileType<FitsReader> fits_reader{
  "fits",
  {"fits", "fit"}
};

}
