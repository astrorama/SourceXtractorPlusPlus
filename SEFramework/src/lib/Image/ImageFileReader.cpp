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

#include "SEFramework/Image/ImageFileReader.h"

namespace SourceXtractor {

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


std::shared_ptr<ImageSource> ImageFileReader::get() {
  if (m_image_index >= 0) {
    return get(m_image_index);
  } else if (m_image_path) {
    return get(*m_image_path);
  }

  return get(0);
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


std::unique_ptr<ImageFileReader> ImageFileReader::create(const std::string& filename) {
  // Get file extension without leading dot
  std::string base_filename = filename;

  // Check for optional [image_path]
  boost::regex suffix_regex(R"((^.*)\[(.*)\]$)");
  boost::smatch match;
  if (boost::regex_match(filename, match, suffix_regex)) {
    base_filename = match[1];
  }

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

