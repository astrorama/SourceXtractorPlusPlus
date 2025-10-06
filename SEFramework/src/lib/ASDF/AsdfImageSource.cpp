/**
 * Copyright © 2019-2025 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
 * AsdfImageSource.cpp
 *
 *  Created on: Sep 03, 2025
 *      Author: embray
 */

#include <asdf.h>

#include "SEFramework/ASDF/AsdfFile.h"
#include "SEFramework/ASDF/AsdfImageSource.h"


namespace SourceXtractor {
AsdfImageSource::AsdfImageSource(const std::string& filename, int image_index,
                                 std::optional<std::string> image_path,
                                 ImageTile::ImageType image_type,
                                 std::shared_ptr<FileManager> manager)
    : m_filename(filename)
    , m_image_type(image_type)
    , m_file_manager(std::move(manager))
    , m_handler(m_file_manager->getFileHandler(filename)) {

  auto acc = m_handler->getAccessor<AsdfFile>();
  auto& file = acc->m_fd;

  if (image_path) {
    m_ndarray = std::move(file.getNdarray(image_path.value()));
  } else {
    // Find the <image_index>'th ndarray that actually looks like a usable image
    int found = 0;
    int ndarray_index = 0;
    while (found <= image_index) {
      try {
        auto ndarray = file.getNdarray(ndarray_index);

        if (ndarray->isSupportedImage()) {
          if (found == image_index) {
            m_ndarray = std::move(ndarray);
            break;
          } else {
            found++;
          }
        }
      } catch (const AsdfFile::AsdfUnsupportedDatatypeException&) {
        // continue
      } catch (const AsdfFile::AsdfValueNotFoundException&) {
        break;
      }
      ndarray_index++;
    }

    if (!m_ndarray) {
      throw AsdfImageSource::InvalidImageNdarrayException();
    }
  }

  uint64_t ndim = m_ndarray->ndim();
  if (ndim < 2 || ndim > 3) {
    if (image_path) {
      throw InvalidImageNdarrayException() << "Can't find 2D image or data cube at "
        << image_path.value() << "in ASDF file: " << filename;
    } else {
      throw InvalidImageNdarrayException() << "Can't find 2D image or data cube in ASDF file: "
        << filename;
    }
  }

  try {
    m_ndarray->getImageType();
  } catch (const AsdfFile::AsdfUnsupportedDatatypeException&) {
    if (image_path) {
      throw InvalidImageNdarrayException() << "Unsupported datatype for ndarray at "
        << image_path.value() << "in ASDF file: " << filename;
    } else {
      throw InvalidImageNdarrayException() << "Unsupported datatype in ASDF file: "
        << filename;
    }
  }
}


void AsdfImageSource::setLayer(int layer) {
  uint64_t depth = m_ndarray->ndim() >= 3 ? m_ndarray->shape().at(0) : 0;
  if (layer < 0 || layer >= static_cast<int>(depth)) {
    throw Elements::Exception() << "Trying to access an inexistent data cube layer (" << layer << ") in " << m_filename;
  }
  m_current_layer = layer;
}


std::shared_ptr<ImageTile> AsdfImageSource::getImageTile(int x, int y, int width, int height) const {
  // TODO: (#5) support image data type conversion
  auto tile = ImageTile::create(m_image_type, x, y, width, height,
                                std::const_pointer_cast<ImageSource>(shared_from_this()));
  m_ndarray->fillImageTile(tile, m_current_layer);
  return tile;
}


void AsdfImageSource::saveTile(ImageTile& /* tile */) {
  throw Elements::Exception() << "Saving to ASDF not yet supported";
};
}
