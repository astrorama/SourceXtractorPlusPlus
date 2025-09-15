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
 * @file SEFramework/FITS/FitsReader.h
 * @date 09/01/25
 * @author embray
 */

#ifndef _SEFRAMEWORK_FITS_FITSREADER_H
#define _SEFRAMEWORK_FITS_FITSREADER_H

#include "SEFramework/Image/BufferedImage.h"
#include "SEFramework/Image/ImageFileReader.h"
#include "SEFramework/Image/ImageTile.h"
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

  using ImageFileReader::get;
  /**
   * Get the N-th image HDU from the FITS file
   *
   * NOTE: For consistency's sake across ImageFileReaders this is 0-indexed, and *not* the
   * 1-indexed FORTRAN/FITS convention.  It also does not correspond to the HDU number itself,
   * but rather the count of HDUs containing valid image arrays.
   *
   * Use FitsReader.getHdu to get by FITS HDU number
   */
  std::shared_ptr<ImageSource> get(int image_index,
                                   ImageTile::ImageType image_type = ImageTile::AutoType) override;
  std::shared_ptr<ImageSource> get(const std::string& image_path,
                                   ImageTile::ImageType image_type = ImageTile::AutoType) override;

  std::shared_ptr<FitsImageSource> getHdu(int hdu_num,
                                          ImageTile::ImageType image_type = ImageTile::AutoType);

  // TODO: Integrate this interface into the ImageFileReader base class
  template <typename T>
  static std::shared_ptr<Image<T>> readFile(const std::string& filename) {
    auto image_source = std::make_shared<FitsImageSource>(filename, 0, ImageTile::getTypeValue(T()));
    return BufferedImage<T>::create(image_source);
  }

private:
  std::map<int, int> m_image_hdu_map;

}; /* End of FitsReader class */

} /* namespace SourceXtractor */


#endif  /* _SEFRAMEWORK_FITS_FITSREADER_H */

