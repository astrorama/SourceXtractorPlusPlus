/** Copyright © 2019-2025 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
 * @file SEFramework/ASDF/AsdfReader.cpp
 * @date 09/12/25
 * @author embray
 */

#include <sstream>

#include <boost/regex.hpp>

#include <ElementsKernel/Exception.h>

#include "SEFramework/ASDF/AsdfFile.h"
#include "SEFramework/ASDF/AsdfReader.h"
#include "SEFramework/Image/ImageFileReader.h"
#include "SEFramework/Image/ImageSource.h"


namespace SourceXtractor {


bool AsdfReader::test(std::istream& stream) {
  // Should be more than enough to determine the #ASDF header comment
  std::string buf(256, '\0');
  if (!stream.read(&buf[0], 256)) {
    return false;
  }

  buf.erase(buf.find_first_of("\r\n"));
  boost::regex asdf_header_regex(R"(^#ASDF \d+\.\d+\.\d+$)");
  return boost::regex_match(buf, asdf_header_regex);
}


std::shared_ptr<ImageSource> AsdfReader::get(int image_index, ImageTile::ImageType image_type) {
  // Unlike a multi-extension FITS file, whose image HDUs are conventionally
  // alternative frames, an ASDF tree routinely bundles ancillary planes beside the
  // science array; e.g. a Roman mosaic carries err, weight, context and two variance
  // planes, none of which is a detection image.  Enumerating them the way the FITS
  // path does would silently run detection on the error map.  Nothing in the file
  // says which array is the image, so refuse to choose rather than guess wrong.
  if (!hasImageSelection()) {
    AsdfFile file{m_filename};
    const auto& paths = file.getImageNdarrayPaths();

    if (paths.size() > 1) {
      // Listed with their indices, which are what a bare integer selects, so either
      // the path or the shorter index can be copied straight out of this message
      std::ostringstream candidates;
      for (size_t i = 0; i < paths.size(); i++) {
        candidates << "\n    [" << i << "]  " << paths[i];
      }

      throw Elements::Exception()
        << "The ASDF file " << m_filename << " holds " << paths.size() << " arrays that "
        << "could be images, and nothing in the file says which one is meant.  Name it "
        << "explicitly, either as " << m_filename << "[<name>] or with the corresponding "
        << "--*-image-name option, choosing from:" << candidates.str();
    }
  }

  return std::make_shared<AsdfImageSource>(m_filename, image_index, image_type,
                                           m_wcs_name);
}


std::shared_ptr<ImageSource> AsdfReader::get(const std::string& extname,
                                             ImageTile::ImageType image_type) {
  return std::make_shared<AsdfImageSource>(m_filename, extname, image_type, m_wcs_name);
}


static ImageFileReader::StaticFileType<AsdfReader> asdf_reader{
  "asdf",
  {"asdf"}
};

}

