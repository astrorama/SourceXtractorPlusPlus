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

/*
 * AsdfFile.cpp
 *
 *  Created on: Aug 14, 2025
 *      Author: embray
 */

#include <assert.h>

#include <iostream>

#include <boost/filesystem/path.hpp>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "SEFramework/ASDF/AsdfFile.h"

namespace SourceXtractor {

static Elements::Logging logger = Elements::Logging::getLogger("AsdfFile");


AsdfFile::AsdfFile(const boost::filesystem::path& path, bool writeable)
    : m_path(path), m_asdf_ptr(nullptr, asdf_close) {

  if (writeable) {
    throw Elements::Exception() << "ASDF files are not currently supported for writing";
  }

  open();
}


AsdfFile::~AsdfFile() {}


asdf_file_t* AsdfFile::getAsdfFilePtr() {
  return m_asdf_ptr.get();
}

void AsdfFile::open() {
  asdf_file_t* ptr = asdf_open_file(m_path.native().c_str(), "r");

  // TODO: Actually kinda stinks there's not enough options for retrieving file open errors
  // yet in libasdf; this needs improvement on the libasdf side.
  if (ptr == nullptr) {
      throw Elements::Exception()
          << "Can't open ASDF file: ";
  } else {
    // Check if the file was created but has an error condition set
    const char *error_message = asdf_error(ptr);

    if (error_message != nullptr) {
      throw Elements::Exception()
          << "Can't open ASDF file: " << m_path.native() << " reason: " << error_message;
    }
  }

  m_asdf_ptr.reset(ptr);
}

// TODO: Support negative indexing as well
AsdfFile::Ndarray AsdfFile::getNdarray(int index) {
  AsdfValuePtr root = getValue("/");

  if (!asdf_value_is_mapping(root.get())) {
    throw AsdfValueNotFoundException() << "ASDF tree root is not a mapping in file: "
      << m_path.native();
  }

  asdf_mapping_iter_t iter = asdf_mapping_iter_init();
  asdf_mapping_item_t* item;
  int count = 0;

  while ((item = asdf_mapping_iter(root.get(), &iter)) != nullptr) {
    asdf_value_t* value = asdf_mapping_item_value(item);
    if (asdf_value_is_ndarray(value)) {
      if (count == index) {
        return Ndarray(shared_from_this(), value);
      } else if (count > index) {
        break;
      }
      count++;
    }
  }

  throw AsdfValueNotFoundException() << "No ndarray at index " << index << " in file: "
    << m_path.native();
}

AsdfFile::Ndarray AsdfFile::getNdarray(const std::string &path) {
  AsdfValuePtr value = getValue(path);
  return Ndarray(shared_from_this(), value.get());
}


AsdfFile::AsdfValuePtr AsdfFile::getValue(const std::string& path) {
  asdf_value_t *v = asdf_get_value(m_asdf_ptr.get(), path.c_str());
  if (!v) {
    throw AsdfValueNotFoundException() << "No value at path " << path << " in file: "
      << m_path.native();
  }

  return AsdfValuePtr(v);
}


AsdfFile::Ndarray::Ndarray(std::shared_ptr<AsdfFile> file, asdf_value_t *value)
    : Ndarray(file, (asdf_ndarray_t*)nullptr) {
  asdf_ndarray_t *ndarray_ptr = nullptr;
  asdf_value_err_t err = asdf_value_as_ndarray(value, &ndarray_ptr);
  switch (err) {
    case ASDF_VALUE_OK:
      // Value exists and is an ndarray: OK
      break;
    case ASDF_VALUE_ERR_TYPE_MISMATCH: {
      const char* path = asdf_value_path(value);
      throw AsdfValueTypeMismatchException() << "Value at " << path << " is not an ndarray: "
        << file->m_path.native();
    }
    default: {
      const char *error_message = asdf_error(file->getAsdfFilePtr());
      throw AsdfValueNotFoundException() << "An error occurred reading the ASDF file "
        << file->m_path.native() << ": " << error_message;
    }
  }
  m_ndarray_ptr = ndarray_ptr;
}


void AsdfFile::Ndarray::fillImageTile(const std::shared_ptr<ImageTile> image_tile, int layer) {
  uint64_t plane_origin = layer;
  void* data = image_tile->getDataPtr();
  asdf_ndarray_err_t err = asdf_ndarray_read_tile_2d(
    m_ndarray_ptr,
    image_tile->getPosX(),
    image_tile->getPosY(),
    image_tile->getWidth(),
    image_tile->getHeight(),
    &plane_origin,
    &data
  );

  switch (err) {
    case ASDF_NDARRAY_OK:
      break;
    case ASDF_NDARRAY_ERR_OUT_OF_BOUNDS:
      throw Elements::Exception() << "requested image tile (" << image_tile->getPosX()
        << ", " << image_tile->getPosY() << ") -> ("
        << image_tile->getPosX() + image_tile->getWidth() << ", "
        << image_tile->getPosY() + image_tile->getHeight() << ") is out of bounds";
    case ASDF_NDARRAY_ERR_OOM:
      throw Elements::Exception() << "out of memory copying image tile";
    case ASDF_NDARRAY_ERR_INVAL:
    default:
      throw Elements::Exception() << "invalid argument to asdf_ndarray_read_tile_2d";
  }
}

}  // namespace SourceXtractor
