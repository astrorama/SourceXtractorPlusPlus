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
std::unique_ptr<AsdfFile::Ndarray> AsdfFile::getNdarray(int index) {
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
        return std::unique_ptr<Ndarray>(new Ndarray(*this, value));
      } else if (count > index) {
        break;
      }
      count++;
    }
  }

  throw AsdfValueNotFoundException() << "No ndarray at index " << index << " in file: "
    << m_path.native();
}


std::unique_ptr<AsdfFile::Ndarray> AsdfFile::getNdarray(const std::string &path) {
  AsdfValuePtr value = getValue(path);
  return std::unique_ptr<Ndarray>(new Ndarray(*this, value.get()));
}


bool fitsWcsValuePredicate(asdf_value_t* value) {
  asdf_gwcs_t* gwcs = nullptr;
  asdf_value_err_t err = asdf_value_as_gwcs(value, &gwcs);

  if (ASDF_VALUE_OK != err || !gwcs) {
    return false;
  }

  return asdf_gwcs_is_fits((asdf_file_t*)asdf_value_file(value), gwcs);
}


std::unique_ptr<AsdfFile::FitsWCS> AsdfFile::getFitsWCS() {
  AsdfValuePtr root = getValue("/");

  if (!root) {
    throw AsdfValueNotFoundException() << "Could not load the root of the ASDF tree in file: "
      << m_path.native();
  }

  // Find the first applicable GWCS, if any
  asdf_find_item_t* item = asdf_value_find(root.get(), fitsWcsValuePredicate);

  if (!item) {
    logger.warn() << "No FITS-compatible WCS could be found in the ASDF file: "
      << m_path.native();
    return std::unique_ptr<AsdfFile::FitsWCS>{};
  }

  asdf_value_t* value = asdf_find_item_value(item);
  return std::unique_ptr<FitsWCS>(new FitsWCS(*this, value));
}


std::unique_ptr<AsdfFile::FitsWCS> AsdfFile::getFitsWCS(const std::string& path) {
  asdf_value_t* value = asdf_get_value(m_asdf_ptr.get(), path.c_str());
  asdf_gwcs_t* gwcs = nullptr;

  if (!value) {
    throw AsdfValueNotFoundException() << "No value at given WCS path " << path
      << " in ASDF file: " << m_path.native();
  }

  asdf_value_err_t err = asdf_value_as_gwcs(value, &gwcs);

  if (ASDF_VALUE_OK != err || !asdf_gwcs_is_fits(m_asdf_ptr.get(), gwcs)) {
    throw AsdfValueNotFoundException() << "Value at given WCS path " << path
      << " is not a FITS-compatible GWCS in ASDF file: " << m_path.native();
  }

  return std::unique_ptr<FitsWCS>(new FitsWCS(*this, value));
}


AsdfFile::AsdfValuePtr AsdfFile::getValue(const std::string& path) {
  asdf_value_t* v = asdf_get_value(m_asdf_ptr.get(), path.c_str());
  if (!v) {
    throw AsdfValueNotFoundException() << "No value at path " << path << " in file: "
      << m_path.native();
  }

  return AsdfValuePtr(v);
}


static ImageTile::ImageType convertDatatypeToImageType(const asdf_datatype_t* datatype) {
  ImageTile::ImageType image_type;

  switch (datatype->type) {
  case ASDF_DATATYPE_FLOAT32:
    image_type = ImageTile::FloatImage;
    break;
  case ASDF_DATATYPE_FLOAT64:
    image_type = ImageTile::DoubleImage;
    break;
  case ASDF_DATATYPE_INT32:
    image_type = ImageTile::IntImage;
    break;
  case ASDF_DATATYPE_UINT32:
    image_type = ImageTile::UIntImage;
    break;
  case ASDF_DATATYPE_INT64:
    image_type = ImageTile::LongLongImage;
    break;
  default:
    // TODO: Support more datatypes supported by ASDF
    // Currently SE++ (and ImageTile::ImageType) is constrainted by the basic BITPIX datatypes
    // supported by FITS.  There's no strong need for that other than the fact that it currently
    // only supports FITS.  Nevertheless for now this will cover most common cases.
    throw AsdfFile::AsdfUnsupportedDatatypeException() << "Unsupported ASDF ndarray datatype: "
      << asdf_ndarray_datatype_to_string(datatype->type);
  }

  return image_type;
}


static asdf_scalar_datatype_t convertImageTypeToDatatype(ImageTile::ImageType image_type) {
  // This is the same default used for FITS
  asdf_scalar_datatype_t datatype = ASDF_DATATYPE_FLOAT32;

  switch (image_type) {
  default:
  case ImageTile::FloatImage:
    datatype = ASDF_DATATYPE_FLOAT32;
    break;
  case ImageTile::DoubleImage:
    datatype = ASDF_DATATYPE_FLOAT64;
    break;
  case ImageTile::IntImage:
    datatype = ASDF_DATATYPE_INT32;
    break;
  case ImageTile::UIntImage:
    datatype = ASDF_DATATYPE_UINT32;
    break;
  case ImageTile::LongLongImage:
    datatype = ASDF_DATATYPE_INT64;
    break;
  }

  return datatype;
}


AsdfFile::Ndarray::Ndarray(const AsdfFile& file, asdf_value_t* value)
    : Ndarray((asdf_ndarray_t*)nullptr) {
  asdf_ndarray_t* ndarray_ptr = nullptr;
  asdf_value_err_t err = asdf_value_as_ndarray(value, &ndarray_ptr);
  const char* path = asdf_value_path(value);
  switch (err) {
    case ASDF_VALUE_OK:
      // Value exists and is an ndarray: OK
      break;
    case ASDF_VALUE_ERR_TYPE_MISMATCH: {
      throw AsdfValueTypeMismatchException() << "Value at " << path << " is not an ndarray: "
        << file.m_path.native();
    }
    default: {
      const char* error_message = asdf_error(file.getAsdfFilePtr());
      throw AsdfValueNotFoundException() << "An error occurred reading the ASDF file "
        << file.m_path.native() << ": " << error_message;
    }
  }
  m_ndarray_ptr = ndarray_ptr;
  m_path = path;
  m_image_type = convertDatatypeToImageType(&ndarray_ptr->datatype);
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
    convertImageTypeToDatatype(image_tile->getType()),
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


bool AsdfFile::Ndarray::isSupportedImage() const {
  uint64_t ndim = m_ndarray_ptr->ndim;

  if (ndim < 2 || ndim > 3) {
    return false;
  }

  try {
    getImageType();
  } catch (const AsdfFile::AsdfUnsupportedDatatypeException&) {
    return false;
  }

  return true;
}


/**
 * NOTE: The asdf_value_t* should be for the full GWCS object, not just the
 * fitswcs_imaging part
 *
 * The full GWCS is needed in order to properly read the FITS WCS out of it.
 */
AsdfFile::FitsWCS::FitsWCS(const AsdfFile& file, asdf_value_t* value)
    : FitsWCS((asdf_gwcs_fits_t*)nullptr) {
  asdf_gwcs_t* gwcs_ptr = nullptr;
  asdf_gwcs_fits_t* gwcs_fits_ptr = nullptr;
  asdf_value_err_t err = asdf_value_as_gwcs(value, &gwcs_ptr);
  switch (err) {
    case ASDF_VALUE_OK:
      // Value exists and is an ndarray: OK
      break;
    case ASDF_VALUE_ERR_TYPE_MISMATCH: {
      const char* path = asdf_value_path(value);
      throw AsdfValueTypeMismatchException() << "Value at " << path << " is not a GWCS: "
        << file.m_path.native();
    }
    default: {
      const char* error_message = asdf_error(file.getAsdfFilePtr());
      throw AsdfValueNotFoundException() << "An error occurred reading the ASDF file "
        << file.m_path.native() << ": " << error_message;
    }
  }

  if (!asdf_gwcs_is_fits(file.getAsdfFilePtr(), gwcs_ptr)) {
      const char* path = asdf_value_path(value);
      throw AsdfValueTypeMismatchException() << "Value at " << path << " does not contain "
        "a FITS-compatible WCS: " << file.m_path.native();
  }

  // This structure is already checked by asdf_gwcs_is_fits so we should expect all
  // these values to be valid now...
  const asdf_gwcs_step_t* step0 = &gwcs_ptr->steps[0];
  gwcs_fits_ptr = (asdf_gwcs_fits_t*)step0->transform;
  assert(gwcs_fits_ptr);

  m_gwcs_fits_ptr = gwcs_fits_ptr;
  // We don't need the asdf_value_t anymore at this point and can release it.
  asdf_value_destroy(value);
}


}  // namespace SourceXtractor
