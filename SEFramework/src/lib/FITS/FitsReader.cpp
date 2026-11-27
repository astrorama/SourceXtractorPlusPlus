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

#include <boost/algorithm/string.hpp>

#include <ElementsKernel/Exception.h>

#include "SEUtils/VariantCast.h"

#include "SEFramework/FITS/FitsFile.h"
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


/**
 * An index is the HDU number, counting the primary HDU as 0
 *
 * This is 0-based numbering the Python interface uses throughout
 * (see PyFitsFile), as opposed to CFITSIO's traditional 1-based
 * indexing.  Raises on a FITS file containing no image HDUs.
 */
std::shared_ptr<ImageSource> FitsReader::get(int image_index, ImageTile::ImageType image_type) {
  return getHdu(image_index + 1, image_type);
}


/**
 * The n'th image HDU, which is what iterating the file yields
 *
 * This is not the HDU index within the file, but rather the index into the
 * ordered subset of HDUs containing images.
 */
std::shared_ptr<ImageSource> FitsReader::at(int image_index, ImageTile::ImageType image_type) {
  // TODO: It turns out none of this is necessary: It's already implemented in
  // the FitsFile class, which actually loops over all the HDUs when opening the file
  // and gets the HDU numbers of images (can be accessed with FitsFile::getImageHdus)
  // so should just use that instead.
  auto it = m_image_hdu_map.find(image_index);

  if (it != m_image_hdu_map.end()) {
    return std::make_shared<FitsImageSource>(m_filename, it->second, image_type);
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
      auto image_source = getHdu(++hdu_num, image_type);
      m_image_hdu_map[++known_index] = hdu_num;

      if (known_index == image_index) {
        return image_source;
      }
    } catch (const FitsImageSource::InvalidHduTypeException&) {
      // Preserving the original behaviour for iteration of MEF files:
      // the sequence of images ends at the first HDU that is not one,
      // rather than continuing to check later HDUs. Wanted to change
      // this to iterate all potential IMAGE HDU, but that breaks the
      // current assumptions.  A dataless primary HDU is the one exception,
      // since it is how a multi-extension file often starts.
      // So a [PRIMARY, SCI, TABLE, DQ] file yields SCI alone.
      //
      // This is purely a convention e.g. of HST products, which put a table
      // between the science and data-quality arrays. Nothing in the FITS
      // standard says image HDUs must be contiguous, so it is worth
      // revisiting, but not as a silent side effect of generalizing the
      // reader. Note the heuristic makes much less sense for ASDF, whose
      // arrays are not an unamibiguously ordered sequence at all.
      if (hdu_num == 1) {
        continue;
      }

      throw FitsImageSource::UnknownHduException()
          << "No image at index " << image_index << " in file " << m_filename
          << ": the images end at HDU " << hdu_num - 1 << ", which is followed by a "
          << "non-image HDU";
    } catch (const FitsImageSource::UnknownHduException&) {
      throw;
    }
  }

  // Shouldn't get here
  throw FitsImageSource::UnknownHduException();
}


std::shared_ptr<ImageSource> FitsReader::get(const std::string& extname,
                                             ImageTile::ImageType image_type) {
  return std::make_shared<FitsImageSource>(m_filename, extname, image_type);
}


std::shared_ptr<FitsImageSource> FitsReader::getHdu(int hdu_num, ImageTile::ImageType image_type) {
  return std::make_shared<FitsImageSource>(m_filename, hdu_num, image_type);
}


std::vector<std::string> FitsReader::getImageNames() {
  FitsFile file{m_filename, false};
  std::vector<std::string> names;

  for (auto hdu : file.getImageHdus()) {
    const auto& headers = file.getHDUHeaders(hdu);
    auto found = headers.find("EXTNAME");

    // A primary HDU, and any extension that was written without one, has no
    // EXTNAME and so can only be addressed by index
    if (found == headers.end()) {
      names.emplace_back();
      continue;
    }

    // The raw value is padded out to the width of the card so trim it
    auto name = VariantCast<std::string>(found->second.m_value);
    boost::trim(name);
    names.push_back(name);
  }

  return names;
}


static ImageFileReader::StaticFileType<FitsReader> fits_reader{
  "fits",
  {"fits", "fit"}
};

}
