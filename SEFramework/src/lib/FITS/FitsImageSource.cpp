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

#include "SEFramework/FITS/FitsImageSource.h"
#include <ElementsKernel/Exception.h>
#include <iomanip>
#include <fstream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>


namespace SourceXtractor {


static std::map<std::string, std::string> loadFitsHeader(fitsfile *fptr) {
  std::map<std::string, std::string> headers;
  char record[81];
  int keynum = 1, status = 0;

  fits_read_record(fptr, keynum, record, &status);
  while (status == 0 && strncmp(record, "END", 3) != 0) {
    static boost::regex regex("(.+)=([^\\/]*)(.*)");
    std::string record_str(record);

    boost::smatch sub_matches;
    if (boost::regex_match(record_str, sub_matches, regex)) {
      auto keyword = boost::to_upper_copy(sub_matches[1].str());
      boost::trim(keyword);
      auto i = headers.emplace(keyword, sub_matches[2]);
      boost::trim(i.first->second);
    }
    fits_read_record(fptr, ++keynum, record, &status);
  }

  return headers;
}

template<typename T>
FitsImageSource<T>::FitsImageSource(const std::string& filename, int hdu_number,
                                    std::shared_ptr<FitsFileManager> manager)
  : m_filename(filename), m_manager(manager), m_hdu_number(hdu_number) {
  int status = 0;
  int bitpix, naxis;
  long naxes[2] = {1,1};

  auto fptr = m_manager->getFitsFile(filename);

  if (m_hdu_number <= 0) {
    int hdunum, hdutype;
    fits_get_num_hdus(fptr, &hdunum, &status);
    if (status != 0) {
      throw Elements::Exception() << "Can't get the number of HDUs in the FITS file: " << filename;
    }

    m_hdu_number = 0;
    do {
      ++m_hdu_number;
      switchHdu(fptr, m_hdu_number);
      fits_get_hdu_type(fptr, &hdutype, &status);
      if (status == 0 && hdutype == IMAGE_HDU) {
        fits_get_img_param(fptr, 2, &bitpix, &naxis, naxes, &status);
      }
      if (status != 0) {
        throw Elements::Exception() << "Can't get the type of the HDU " << m_hdu_number << " in the FITS file: "
                                    << filename;
      }
    } while (m_hdu_number <=hdunum && !(hdutype == IMAGE_HDU && naxis == 2));
    if (hdutype != IMAGE_HDU) {
      throw Elements::Exception() << "Can't find 2D image in FITS file: " << filename;
    }
  }
  else {
    switchHdu(fptr, m_hdu_number);
  }

  fits_get_img_param(fptr, 2, &bitpix, &naxis, naxes, &status);
  if (status != 0 || naxis != 2) {
    throw Elements::Exception() << "Can't find 2D image in FITS file: " << filename << "[" << m_hdu_number << "]";
  }

  m_width = naxes[0];
  m_height = naxes[1];

  m_header = loadFitsHeader(fptr);
  loadHeadFile();
}


template<typename T>
FitsImageSource<T>::FitsImageSource(const std::string& filename, int width, int height,
                                    const std::shared_ptr<CoordinateSystem> coord_system,
                                    std::shared_ptr<FitsFileManager> manager)
  : m_filename(filename), m_manager(manager), m_hdu_number(1) {
  m_width = width;
  m_height = height;

  {
    // CREATE NEW FITS FILE
    int status = 0;
    fitsfile* fptr = nullptr;
    fits_create_file(&fptr, ("!"+filename).c_str(), &status);
    if (status != 0) {
      throw Elements::Exception() << "Can't create or overwrite FITS file: " << filename;
    }
    assert(fptr != nullptr);

    long naxes[2] = {width, height};
    fits_create_img(fptr, getImageType(), 2, naxes, &status);

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

    std::vector<T> buffer(width);
    for (int i = 0; i<height; i++) {
      long first_pixel[2] = {1, i+1};
      fits_write_pix(fptr, getDataType(), first_pixel, width, &buffer[0], &status);
    }
    fits_close_file(fptr, &status);

    if (status != 0) {
      throw Elements::Exception() << "Couldn't allocate space for new FITS file: " << filename;
    }
  }

  auto fptr = m_manager->getFitsFile(filename, true);
  switchHdu(fptr, m_hdu_number);
}


template<typename T>
std::shared_ptr<ImageTile<T>> FitsImageSource<T>::getImageTile(int x, int y, int width, int height) const {
  auto fptr = m_manager->getFitsFile(m_filename);
  switchHdu(fptr, m_hdu_number);

  auto tile = std::make_shared<ImageTile<T>>((const_cast<FitsImageSource *>(this))->shared_from_this(), x, y, width,
                                             height);

  long first_pixel[2] = {x + 1, y + 1};
  long last_pixel[2] = {x + width, y + height};
  long increment[2] = {1, 1};
  int status = 0;

  auto image = tile->getImage();
  fits_read_subset(fptr, getDataType(), first_pixel, last_pixel, increment,
                   nullptr, &image->getData()[0], nullptr, &status);
  if (status != 0) {
    throw Elements::Exception() << "Error reading image tile from FITS file.";
  }

  return tile;
}


template<typename T>
void FitsImageSource<T>::saveTile(ImageTile<T>& tile) {
  auto fptr = m_manager->getFitsFile(m_filename, true);
  switchHdu(fptr, m_hdu_number);

  auto image = tile.getImage();

  int x = tile.getPosX();
  int y = tile.getPosY();
  int width = image->getWidth();
  int height = image->getHeight();

  long first_pixel[2] = {x + 1, y + 1};
  long last_pixel[2] = {x + width, y + height};
  int status = 0;

  fits_write_subset(fptr, getDataType(), first_pixel, last_pixel, &image->getData()[0], &status);
  if (status != 0) {
    throw Elements::Exception() << "Error saving image tile to FITS file.";
  }
}


template<typename T>
void FitsImageSource<T>::switchHdu(fitsfile *fptr, int hdu_number) const {
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


template<typename T>
void FitsImageSource<T>::loadHeadFile() {
  auto filename = boost::filesystem::path(m_filename);
  auto base_name = filename.stem();
  base_name.replace_extension(".head");
  auto head_filename = filename.parent_path() / base_name;

  int current_hdu = 1;

  if (boost::filesystem::exists(head_filename)) {
    std::ifstream file;

    // open the file and check
    file.open(head_filename.native());
    if (!file.good() || !file.is_open()) {
      throw Elements::Exception() << "Cannot load ascii header file: " << head_filename;
    }

    while (file.good()) {
      std::string line;
      std::getline(file, line);

      line = boost::regex_replace(line, boost::regex("\\s*#.*"), std::string(""));
      line = boost::regex_replace(line, boost::regex("\\s*$"), std::string(""));

      if (line.size() == 0) {
        continue;
      }

      if (boost::to_upper_copy(line) == "END") {
        current_hdu++;
      }
      else if (current_hdu == m_hdu_number) {
        boost::smatch sub_matches;
        if (boost::regex_match(line, sub_matches, boost::regex("(.+)=(.+)")) && sub_matches.size() == 3) {
          auto keyword = boost::to_upper_copy(sub_matches[1].str());
          m_header[keyword] = sub_matches[2];
        }
      }
    }
  }
}


template <>
int FitsImageSource<double>::getDataType() const { return TDOUBLE; }

template <>
int FitsImageSource<float>::getDataType() const { return TFLOAT; }

template <>
int FitsImageSource<unsigned int>::getDataType() const { return TUINT; }

template <>
int FitsImageSource<int>::getDataType() const { return TINT; }

//FIXME what if compiled on 32bit system?
template <>
int FitsImageSource<long>::getDataType() const { return TLONGLONG; }

template <>
int FitsImageSource<long long>::getDataType() const { return TLONGLONG; }

template <>
int FitsImageSource<double>::getImageType() const { return DOUBLE_IMG; }

template <>
int FitsImageSource<float>::getImageType() const { return FLOAT_IMG; }

template <>
int FitsImageSource<unsigned int>::getImageType() const { return LONG_IMG; }

template <>
int FitsImageSource<int>::getImageType() const { return LONG_IMG; }

//FIXME what if compiled on 32bit system?
template <>
int FitsImageSource<long>::getImageType() const { return LONGLONG_IMG; }

template <>
int FitsImageSource<long long>::getImageType() const { return LONGLONG_IMG; }

//FIXME add missing types


/// Instantiation
template class FitsImageSource<SeFloat>;
template class FitsImageSource<int64_t>;
template class FitsImageSource<unsigned int>;

}
