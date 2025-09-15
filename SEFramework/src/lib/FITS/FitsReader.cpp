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
 * @file SEFramework/FITS/FitsReader.cpp
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


std::shared_ptr<ImageSource> FitsReader::get(int image_index) {
  // TODO: It turns out none of this is necessary: It's already implemented in
  // the FitsFile class, which actually loops over all the HDUs when opening the file
  // and gets the HDU numbers of images (can be accessed with FitsFile::getImageHdus)
  // so should just use that instead.
  auto it = m_image_hdu_map.find(image_index);

  if (it != m_image_hdu_map.end()) {
    return std::make_shared<FitsImageSource>(m_filename, it->second);
  }

  // Try loading HDUs from the file until we find the next image HDU
  int hdu_num = 0;
  int known_index = -1;

  if (!m_image_hdu_map.empty()) {
    auto rit = m_image_hdu_map.rbegin();
    known_index = rit->first;
    hdu_num = rit->second;
  }

  while (known_index < image_index) {
    try {
      auto image_source = getHdu(++hdu_num);
      m_image_hdu_map[++known_index] = hdu_num;

      if (known_index == image_index) {
        return image_source;
      }
    } catch (const FitsImageSource::InvalidHduTypeException&) {
      continue;
    } catch (const FitsImageSource::UnknownHduException&) {
      throw;
    }
  }

  // Shouldn't get here
  throw FitsImageSource::UnknownHduException();
}


std::shared_ptr<ImageSource> FitsReader::get(const std::string& extname) {
  return std::make_shared<FitsImageSource>(m_filename, extname);
}


std::shared_ptr<FitsImageSource> FitsReader::getHdu(int hdu_num) {
  return std::make_shared<FitsImageSource>(m_filename, hdu_num);
}


static ImageFileReader::StaticFileType<FitsReader> fits_reader{
  "fits",
  {"fits", "fit"}
};

}
