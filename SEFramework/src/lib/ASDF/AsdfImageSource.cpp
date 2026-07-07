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
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <asdf.h>

#include "ElementsKernel/Logging.h"

#include "SEFramework/ASDF/AsdfFile.h"
#include "SEFramework/ASDF/AsdfImageSource.h"


namespace SourceXtractor {

static auto logger = Elements::Logging::getLogger("ASDF");

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
  auto tile = ImageTile::create(m_image_type, x, y, width, height,
                                std::const_pointer_cast<ImageSource>(shared_from_this()));
  m_ndarray->fillImageTile(tile, m_current_layer);
  return tile;
}


void AsdfImageSource::saveTile(ImageTile& /* tile */) {
  throw Elements::Exception() << "Saving to ASDF not yet supported";
};


/**
 * Prefix an ASDF path with '/' if it doesn't already have, to ensure all paths are
 * normalized; also trims whitespace
 */
inline std::string asdfNormalizePath(std::string s) {
    s = boost::trim_copy(s);
    if (!s.empty() && s.front() != '/') {
      s.insert(s.begin(), '/');
    }
    return s;
}


AsdfImageSource::WcsPathMap AsdfImageSource::parseWcsPath(const std::string& wcs_path) const {
  WcsPathMap path_map;

  size_t nsep = std::count(wcs_path.begin(), wcs_path.end(), ':');

  // Simple case, a single WCS which we indicate with '*'
  if (nsep == 0) {
    path_map["*"] = asdfNormalizePath(wcs_path);
    return path_map;
  }

  std::vector<std::string> tokens;
  boost::split(tokens, wcs_path, boost::is_any_of(":"));
  for (auto& token : tokens) {
    boost::trim(token);
  }

  if (tokens.size() % 2 != 0) {
    throw Elements::Exception() << "Invalid WCS path given: " << wcs_path << "; odd number of "
      "path elements";
  }

  for (size_t idx = 0; idx < tokens.size(); idx += 2) {
    std::string key = asdfNormalizePath(tokens[idx]);
    std::string value = asdfNormalizePath(tokens[idx + 1]);
    path_map[key] = value;
  }

  return path_map;
}


std::unique_ptr<AsdfFile::FitsWCS> AsdfImageSource::getFitsWCS() const {
  auto acc = m_handler->getAccessor<AsdfFile>();
  auto& file = acc->m_fd;
  return file.getFitsWCS();
}

// TODO: Implement handling of wcs_path_map
std::unique_ptr<AsdfFile::FitsWCS> AsdfImageSource::getFitsWCS(std::optional<std::string> wcs_path) const {
  auto acc = m_handler->getAccessor<AsdfFile>();
  auto& file = acc->m_fd;

  if (wcs_path == std::nullopt) {
    return file.getFitsWCS();
  }

  WcsPathMap path_map = parseWcsPath(*wcs_path);
  const std::string& ndarray_path = m_ndarray->getPath();

  if (path_map.find(ndarray_path) != path_map.end()) {
    return file.getFitsWCS(path_map.find(ndarray_path)->second);
  } else if (path_map.find("*") != path_map.end()) {
    return file.getFitsWCS(path_map.find("*")->second);
  } else {
    logger.warn() << "No WCS path found for ndarray at " << ndarray_path << "; trying any WCS";
    return file.getFitsWCS();
  }
}
}
