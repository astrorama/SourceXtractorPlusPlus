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
 * @file SEFramework/ASDF/AsdfReader.h
 * @date 09/12/25
 * @author embray
 */

#ifndef _SEFRAMEWORK_ASDF_ASDFREADER_H
#define _SEFRAMEWORK_ASDF_ASDFREADER_H

#include "SEFramework/Image/BufferedImage.h"
#include "SEFramework/Image/ImageFileReader.h"
#include "SEFramework/Image/ImageTile.h"
#include "SEFramework/ASDF/AsdfImageSource.h"

namespace SourceXtractor {

/**
 * @class AsdfReader
 * @brief
 *
 */
class AsdfReader : public ImageFileReader {

public:
  using ImageFileReader::ImageFileReader;

  static bool test(std::istream& stream);

  using ImageFileReader::get;
  /**
   * Get the N-th supported image ndarray from the ASDF file
   */
  std::shared_ptr<ImageSource> get(int image_index,
                                   ImageTile::ImageType image_type = ImageTile::AutoType) override;
  std::shared_ptr<ImageSource> get(const std::string& image_path,
                                   ImageTile::ImageType image_type = ImageTile::AutoType) override;
}; /* End of AsdfReader class */

} /* namespace SourceXtractor */


#endif  /* _SEFRAMEWORK_ASDF_ASDFREADER_H */

