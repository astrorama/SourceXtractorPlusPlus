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
 * @file ImageFileReader.cpp
 * @date Aug 14, 2025
 * @author embray
 */

#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include <ElementsKernel/Exception.h>
#include <ElementsKernel/Logging.h>

#include "SEFramework/Image/ImageFileReader.h"

namespace SourceXtractor {

static auto logger = Elements::Logging::getLogger("ImageFileReader");

// We use this idiom to make sure the map is initialized on the first need
// Otherwise, this may be initialized *after* the concrete implementations try to register themselves
static std::map<std::string, ImageFileReader::ImageFileType>& getFileTypeMap() {
  static std::map<std::string, ImageFileReader::ImageFileType> file_type_map;
  return file_type_map;
}


static std::map<std::string, ImageFileReader::ImageFileType>& getExtensionMap() {
  static std::map<std::string, ImageFileReader::ImageFileType> extension_map;
  return extension_map;
}


ImageFileReader::ImageFileReader(const std::string& filename) {
  std::string base_filename = filename;
  std::optional<std::string> image_path;

  // Check for optional [image_path]
  boost::regex suffix_regex(R"((^.*)\[(.*)\]$)");
  boost::smatch match;
  if (boost::regex_match(filename, match, suffix_regex)) {
    base_filename = match[1];
    image_path = match[2];
  }

  if (image_path) {
    m_filename = base_filename;

    try {
      size_t idx;
      int image_index = std::stoi(*image_path, &idx);
      if (idx == image_path->size()) {
        m_image_path = std::nullopt;
        m_image_index = image_index;
        return;
      }
    } catch (...) {
      // Not an integer, treat as string
    }

    m_image_path = image_path;
    m_image_index = -1;
  } else {
    m_filename = base_filename;
    m_image_path = std::nullopt;
    m_image_index = -1;
  }
}


ImageFileReader::ImageFileReader(const std::string& filename, const std::string& image_path)
    : m_filename(filename), m_image_path(image_path), m_image_index(-1) {}

ImageFileReader::ImageFileReader(const std::string& filename, int image_index)
    : m_filename(filename), m_image_index(image_index) {}


std::shared_ptr<ImageSource> ImageFileReader::get(ImageTile::ImageType image_type) {
  if (m_image_index >= 0) {
    return get(m_image_index, image_type);
  } else if (m_image_path) {
    return get(*m_image_path, image_type);
  }

  return get(0, image_type);
}


void ImageFileReader::Iterator::advance() {
  // If m_reader is null the iterator is considered "done"
  if (nullptr == m_reader) {
    m_current = nullptr;
    return;
  }

  // A file[image] suffix selects which images the file offers, so iterate over
  // the selection rather than over everything.  A selection names a single image
  // today; one that could cover several (a wildcard, say) would instead return
  // the m_index'th image it selects here
  if (m_reader->hasImageSelection()) {
    m_current = (0 == m_index) ? m_reader->get(m_image_type) : nullptr;
    return;
  }

  try {
    m_current = m_reader->get(m_index, m_image_type);
  } catch (const std::exception& e) {
    // Running off the end of the file is how iteration terminates, so a failure
    // past the first image just ends it.  The first image is different: there is
    // no earlier one that succeeded, so reporting "no images" would hide the
    // real reason the file could not be read
    if (0 == m_index) {
      throw;
    }

    logger.debug() << "Stopping iteration of " << m_reader->m_filename << " at image "
                   << m_index << ": " << e.what();
    m_current = nullptr;
  }
}


void ImageFileReader::registerFileType(ImageFileType file_type) {
  auto lower_name = boost::algorithm::to_lower_copy(file_type.name);
  auto& type_map = getFileTypeMap();
  if (type_map.find(lower_name) != type_map.end()) {
      throw std::runtime_error("File type already registered: " + file_type.name);
  }

  type_map.emplace(lower_name, file_type);

  auto& ext_map = getExtensionMap();
  // Also populate extension lookup map
  for (auto& extension : file_type.extensions) {
    auto lower_ext = boost::algorithm::to_lower_copy(extension);
    if (ext_map.find(lower_ext) != ext_map.end()) {
      throw std::runtime_error("File extension already registered: " + extension);
    }
    ext_map.emplace(extension, file_type);
  }
}


std::vector<std::string> ImageFileReader::getFileTypes() {
  std::vector<std::string> keys;
  for (auto &e : getFileTypeMap()) {
    keys.emplace_back(e.first);
  }
  return keys;
}


std::string ImageFileReader::getDefault() {
  return "fits";
}


static std::unique_ptr<ImageFileReader> tryCreateFromFile(
    const std::string& base_filename, const std::string& filename,
    ImageFileReader::ImageFileType& file_type) {
  std::ifstream ifs(base_filename, std::ios::binary);
  if (ifs && file_type.test(ifs)) {
      return file_type.factory(filename);
  }
  return nullptr;
}


void ImageFileReader::setWcsPath(std::optional<std::string> wcs_path) {
  if (wcs_path && !supportsWcsPath()) {
    logger.warn() << "Selecting a WCS by path is not supported for this file format; "
                  << "ignoring the WCS path given for " << m_filename;
    return;
  }

  m_wcs_path = std::move(wcs_path);
}


std::unique_ptr<ImageFileReader> ImageFileReader::create(const std::string& filename) {
  // Get file extension without leading dot
  std::string base_filename = filename;

  // Check for optional [image_path]
  boost::regex suffix_regex(R"((^.*)\[(.*)\]$)");
  boost::smatch match;
  if (boost::regex_match(filename, match, suffix_regex)) {
    base_filename = match[1];
  }

  // Diagnose an inaccessible file here: every ImageFileType::test() below works on an open stream,
  // so otherwise all of them fail and the file is reported as having an unrecognized format
  boost::system::error_code ec;
  auto status = boost::filesystem::status(base_filename, ec);
  if (ec || !boost::filesystem::exists(status)) {
    throw Elements::Exception() << "Image file " << base_filename << " does not exist";
  }
  if (boost::filesystem::is_directory(status)) {
    throw Elements::Exception() << "Image file " << base_filename << " is a directory";
  }
  std::ifstream ifs(base_filename, std::ios::binary);
  if (!ifs) {
    throw Elements::Exception() << "Image file " << base_filename << " could not be opened for reading";
  }
  ifs.close();

  std::string ext = boost::filesystem::path(base_filename).extension().string();
  if (!ext.empty() && ext[0] == '.') {
    ext.erase(0, 1);
  }
  std::string ext_lower = boost::algorithm::to_lower_copy(ext);

  // Try match by filename extension first
  auto& ext_map = getExtensionMap();
  auto entry = ext_map.find(ext_lower);
  if (entry != ext_map.end()) {
    if (auto img = tryCreateFromFile(base_filename, filename, entry->second)) {
      return img;
    }
  }

  auto& type_map = getFileTypeMap();

  // Try the default type
  std::string default_type = getDefault();
  entry = type_map.find(default_type);
  if (entry != ext_map.end()) {
    if (auto img = tryCreateFromFile(base_filename, filename, entry->second)) {
      return img;
    }
  }

  // Try all known file types
  for (auto& it : type_map) {
    if (auto img = tryCreateFromFile(base_filename, filename, it.second)) {
      return img;
    }
  }

  throw Elements::Exception() << "File type of " << base_filename << " could not be determined";
}

} // end namespace SourceXtractor

