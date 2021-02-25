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
 * FitsImageSource.cpp
 *
 *  Created on: Mar 21, 2018
 *      Author: mschefer
 */

#include <iomanip>
#include <fstream>
#include <numeric>
#include <string>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <ElementsKernel/Exception.h>

#include "SEFramework/FITS/FitsFile.h"
#include "SEUtils/VariantCast.h"

#include "SEFramework/FITS/FitsImageSource.h"

namespace SourceXtractor {

namespace {
}

FitsImageSource::FitsImageSource(const std::string& filename, int hdu_number, ImageTile::ImageType image_type, std::shared_ptr<FitsFileManager> manager)
  : m_filename(filename), m_manager(manager), m_hdu_number(hdu_number) {
  int status = 0;
  int bitpix, naxis;
  long naxes[2] = {1,1};

  m_fits_file = m_manager->getFitsFile(filename);
  auto fptr = m_fits_file->getFitsFilePtr();

  if (m_hdu_number <= 0) {
    if (fits_get_hdu_num(fptr.get(), &m_hdu_number) < 0) {
      throw Elements::Exception() << "Can't get the active HDU from the FITS file: " << filename;
    }
  }
  else {
    switchHdu(fptr.get(), m_hdu_number);
  }

  fits_get_img_param(fptr.get(), 2, &bitpix, &naxis, naxes, &status);
  if (status != 0 || naxis != 2) {
    throw Elements::Exception() << "Can't find 2D image in FITS file: " << filename << "[" << m_hdu_number << "]";
  }

  m_width = naxes[0];
  m_height = naxes[1];

  if (image_type < 0) {
    switch (bitpix) {
    case FLOAT_IMG:
      m_image_type = ImageTile::FloatImage;
      break;
    case DOUBLE_IMG:
      m_image_type = ImageTile::DoubleImage;
      break;
    case LONG_IMG:
      m_image_type = ImageTile::IntImage;
      break;
    case ULONG_IMG:
      m_image_type = ImageTile::UIntImage;
      break;
    case LONGLONG_IMG:
      m_image_type = ImageTile::LongLongImage;
      break;
    default:
      throw Elements::Exception() << "Unsupported FITS image type: " << bitpix;
    }
  } else {
    m_image_type = image_type;
  }
}


FitsImageSource::FitsImageSource(const std::string& filename, int width, int height, ImageTile::ImageType image_type,
                                    const std::shared_ptr<CoordinateSystem> coord_system, bool append,
                                    std::shared_ptr<FitsFileManager> manager)
  : m_filename(filename), m_manager(manager), m_width(width), m_height(height), m_image_type(image_type) {
  {

    m_manager->closeAndPurgeFile(filename);

    int status = 0;
    fitsfile* fptr = nullptr;

    if (append) {
      fits_open_image(&fptr, filename.c_str(), READWRITE, &status);
      if (status != 0) {
        status = 0;
        fits_create_file(&fptr, filename.c_str(), &status);
        if (status != 0) {
          throw Elements::Exception() << "Can't open or create FITS file to add HDU: " << filename;
        }
      }
    } else {
      // Overwrite file
      fits_create_file(&fptr, ("!"+filename).c_str(), &status);
      if (status != 0) {
        throw Elements::Exception() << "Can't create or overwrite FITS file: " << filename;
      }
    }
    assert(fptr != nullptr);

    long naxes[2] = {width, height};
    fits_create_img(fptr, getImageType(), 2, naxes, &status);

    if (fits_get_hdu_num(fptr, &m_hdu_number) < 0) {
      throw Elements::Exception() << "Can't get the active HDU from the FITS file: " << filename;
    }

    int hdutype = 0;
    fits_movabs_hdu(fptr, m_hdu_number, &hdutype, &status);

    if (coord_system) {
      auto headers = coord_system->getFitsHeaders();
      for (auto& h : headers) {
        std::ostringstream padded_key, serializer;
        padded_key << std::setw(8) << std::left << h.first;

        serializer << padded_key.str() << "= " << std::left << std::setw(70) << h.second;
        auto str = serializer.str();

        fits_update_card(fptr, padded_key.str().c_str(), str.c_str(), &status);
        if (status != 0) {
          char err_txt[31];
          fits_get_errstatus(status, err_txt);
          throw Elements::Exception() << "Couldn't write the WCS headers (" << err_txt << "): " << str;
        }
      }
    }

    std::vector<char> buffer(width * ImageTile::getTypeSize(image_type));
    for (int i = 0; i<height; i++) {
      long first_pixel[2] = {1, i+1};
      fits_write_pix(fptr, getDataType(), first_pixel, width, &buffer[0], &status);
    }
    fits_close_file(fptr, &status);

    if (status != 0) {
      throw Elements::Exception() << "Couldn't allocate space for new FITS file: " << filename;
    }
  }

  // Reopens the newly created file through theFitsFileManager
  m_fits_file = m_manager->getFitsFile(filename, true);
  auto fptr = m_fits_file->getFitsFilePtr();
  switchHdu(fptr.get(), m_hdu_number);
}

std::shared_ptr<ImageTile> FitsImageSource::getImageTile(int x, int y, int width, int height) const {
  auto fptr = m_fits_file->getFitsFilePtr();
  switchHdu(fptr.get(), m_hdu_number);

  auto tile = ImageTile::create(m_image_type, x, y, width, height,
      std::const_pointer_cast<ImageSource>(shared_from_this()));

  long first_pixel[2] = {x + 1, y + 1};
  long last_pixel[2] = {x + width, y + height};
  long increment[2] = {1, 1};
  int status = 0;

  fits_read_subset(fptr.get(), getDataType(), first_pixel, last_pixel, increment,
                   nullptr, tile->getDataPtr(), nullptr, &status);
  if (status != 0) {
    throw Elements::Exception() << "Error reading image tile from FITS file.";
  }

  return tile;
}

void FitsImageSource::saveTile(ImageTile& tile) {
    auto fptr = m_fits_file->getFitsFilePtr();
    switchHdu(fptr.get(), m_hdu_number);

    int x = tile.getPosX();
    int y = tile.getPosY();
    int width = tile.getWidth();
    int height = tile.getHeight();

    long first_pixel[2] = {x + 1, y + 1};
    long last_pixel[2] = {x + width, y + height};
    int status = 0;

    fits_write_subset(fptr.get(), getDataType(), first_pixel, last_pixel, tile.getDataPtr(), &status);
    if (status != 0) {
      throw Elements::Exception() << "Error saving image tile to FITS file.";
    }
}

void FitsImageSource::switchHdu(fitsfile *fptr, int hdu_number) const {
  int status = 0;
  int hdu_type = 0;

  fits_movabs_hdu(fptr, hdu_number, &hdu_type, &status);

  if (status != 0) {
    throw Elements::Exception() << "Could not switch to HDU # " << hdu_number << " in file " << m_filename;
  }
  if (hdu_type != IMAGE_HDU) {
    throw Elements::Exception() << "Trying to access non-image HDU in file " << m_filename;
  }
}



std::unique_ptr<std::vector<char>> FitsImageSource::getFitsHeaders(int& number_of_records) const {
  number_of_records = 0;
  std::string records;

  auto& headers = getMetadata();
  for (auto record : headers) {
    auto key = record.first;

    std::string record_string(key);
    if (record_string.size() > 8) {
      throw Elements::Exception() << "FITS keyword longer than 8 characters";
    } else if (record_string.size() < 8) {
      record_string += std::string(8 - record_string.size(), ' ');
    }

    if (headers.at(key).m_value.type() == typeid(std::string)) {
      record_string += "= '" + VariantCast<std::string>(headers.at(key).m_value) + "'";
    }
    else {
      record_string += "= " + VariantCast<std::string>(headers.at(key).m_value);
    }

    if (record_string.size() > 80) {
      throw Elements::Exception() << "FITS record longer than 80 characters";
    }


    if (record_string.size() < 80) {
      record_string += std::string(80 - record_string.size(), ' ');
    }

    records += record_string;
    number_of_records++;
  }

  std::string record_string("END");
  record_string += std::string(80 - record_string.size(), ' ');
  records += record_string;

  std::unique_ptr<std::vector<char>> buffer(new std::vector<char>(records.begin(), records.end()));
  buffer->emplace_back(0);
  return buffer;
}

void FitsImageSource::setMetadata(std::string key, MetadataEntry value) {
  auto fptr = m_fits_file->getFitsFilePtr();
  switchHdu(fptr.get(), m_hdu_number);

  std::ostringstream padded_key, serializer;
  padded_key << std::setw(8) << std::left << key;

  serializer << padded_key.str() << "= " << std::left << std::setw(70) << VariantCast<std::string>(value.m_value);
  auto str = serializer.str();

  int status = 0;
  fits_update_card(fptr.get(), padded_key.str().c_str(), str.c_str(), &status);

  if (status != 0) {
    char err_txt[31];
    fits_get_errstatus(status, err_txt);
    throw Elements::Exception() << "Couldn't write the metadata (" << err_txt << "): " << str;
  }

  // update the metadata
  m_fits_file->getHDUHeaders(m_hdu_number)[key] = value;
}

int FitsImageSource::getDataType() const {
  switch (m_image_type) {
  default:
  case ImageTile::FloatImage:
    return TFLOAT;
  case ImageTile::DoubleImage:
    return TDOUBLE;
  case ImageTile::IntImage:
    return TINT;
  case ImageTile::UIntImage:
    return TUINT;
  case ImageTile::LongLongImage:
    return TLONGLONG;
  }
}

int FitsImageSource::getImageType() const {
  switch (m_image_type) {
  default:
  case ImageTile::FloatImage:
    return FLOAT_IMG;
  case ImageTile::DoubleImage:
    return DOUBLE_IMG;
  case ImageTile::IntImage:
    return LONG_IMG;
  case ImageTile::UIntImage:
    return ULONG_IMG;
  case ImageTile::LongLongImage:
    return LONGLONG_IMG;
  }
}

//FIXME add missing types

}
