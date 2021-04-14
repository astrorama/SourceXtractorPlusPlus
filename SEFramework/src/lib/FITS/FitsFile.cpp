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
 * FitsFile.cpp
 *
 *  Created on: Jun 9, 2020
 *      Author: mschefer
 */

#include <assert.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>

#include "ElementsKernel/Exception.h"
#include "SEFramework/FITS/FitsFile.h"

namespace SourceXtractor {

/**
 * Cast a string to a C++ type depending on the format of the content.
 * - if only digits are present, it will be casted to int64_t
 * - if it matches the regex (one dot and/or exponent present), it will be casted to double
 * - if there is one single character, it will be casted to char
 * - anything else will be casted to std::string, removing simple quotes if necessary
 */
static typename MetadataEntry::value_t valueAutoCast(const std::string& value) {
  boost::regex float_regex("^[-+]?(\\d*\\.?\\d+|\\d+\\.?\\d*)([eE][-+]?\\d+)?$");
  boost::regex int_regex("^[-+]?\\d+$");

  try {
    if (value.empty()) {
      return value;
    } else if (boost::regex_match(value, int_regex)) {
      return static_cast<int64_t>(std::stoll(value));
    } else if (boost::regex_match(value, float_regex)) {
      return std::stod(value);
    } else if (value.size() == 1) {
      return value.at(0);
    }
  } catch (...) {
  }

  // Single quotes are used as escape code of another single quote, and
  // the string starts and ends with single quotes.
  // We used to use boost::io::quoted here, but it seems that starting with 1.73 it
  // does not work well when the escape code and the delimiter are the same
  std::string unquoted;
  bool        escape = false;
  unquoted.reserve(value.size());
  for (auto i = value.begin(); i != value.end(); ++i) {
    if (*i == '\'' && !escape) {
      escape = true;
      // skip this char
    } else {
      escape = false;
      unquoted.push_back(*i);
    }
  }
  return unquoted;
}

static void close_fits(fitsfile* ptr) {
  int status = 0;
  fits_close_file(ptr, &status);
}

FitsFile::FitsFile(const boost::filesystem::path& path, bool writeable)
    : m_path(path), m_is_writeable(writeable), m_fits_ptr(nullptr, close_fits) {
  open();
}

FitsFile::~FitsFile() {}

fitsfile* FitsFile::getFitsFilePtr() {
  return m_fits_ptr.get();
}

const std::vector<int>& FitsFile::getImageHdus() const {
  return m_image_hdus;
}

std::map<std::string, MetadataEntry>& FitsFile::getHDUHeaders(int hdu) {
  return m_headers.at(hdu - 1);
}

void FitsFile::open() {
  int       status = 0;
  fitsfile* ptr    = nullptr;

  // Open
  fits_open_image(&ptr, m_path.native().c_str(), m_is_writeable ? READWRITE : READONLY, &status);
  if (status != 0) {
    throw Elements::Exception() << "Can't open FITS file: " << m_path;
  }

  m_fits_ptr = std::move(std::unique_ptr<fitsfile, void (*)(fitsfile*)>(ptr, [](fitsfile* ptr) {
    int status = 0;
    fits_close_file(ptr, &status);
  }));

  // save current HDU (if the file is opened with advanced cfitsio syntax it might be set already
  int original_hdu = 0;
  fits_get_hdu_num(ptr, &original_hdu);

  // Number of HDU
  m_image_hdus.clear();
  int number_of_hdus = 0;
  if (fits_get_num_hdus(ptr, &number_of_hdus, &status) < 0) {
    throw Elements::Exception() << "Can't get the number of HDUs in FITS file: " << m_path;
  }

  m_headers.resize(number_of_hdus);

  // loop over HDUs to determine which ones are images
  int hdu_type = 0;
  for (int hdu_number = 1; hdu_number <= number_of_hdus; ++hdu_number) {
    fits_movabs_hdu(ptr, hdu_number, &hdu_type, &status);
    if (status != 0) {
      throw Elements::Exception() << "Can't switch HDUs while opening: " << m_path;
    }

    if (hdu_type == IMAGE_HDU) {
      int  bitpix, naxis;
      long naxes[2] = {1, 1};

      fits_get_img_param(ptr, 2, &bitpix, &naxis, naxes, &status);
      if (status == 0 && naxis == 2) {
        m_image_hdus.emplace_back(hdu_number);
      }
    }
  }

  // go back to saved HDU
  fits_movabs_hdu(ptr, original_hdu, &hdu_type, &status);

  // load all FITS headers
  loadFitsHeader();

  // load optional .head file to override headers
  loadHeadFile();
}

static std::map<std::string, MetadataEntry> loadHeadersFromFits(fitsfile* fptr) {
  std::map<std::string, MetadataEntry> headers;
  char                                 record[81];
  int                                  keynum = 1, status = 0;

  fits_read_record(fptr, keynum, record, &status);
  while (status == 0 && strncmp(record, "END", 3) != 0) {
    static boost::regex regex("([^=]{8})=([^\\/]*)(\\/(.*))?");
    std::string         record_str(record);

    boost::smatch sub_matches;
    if (boost::regex_match(record_str, sub_matches, regex)) {
      auto keyword = boost::to_upper_copy(sub_matches[1].str());
      auto value   = sub_matches[2].str();
      auto comment = sub_matches[4].str();
      boost::trim(keyword);
      boost::trim(value);
      boost::trim(comment);
      headers.emplace(keyword, MetadataEntry{valueAutoCast(value), {{"comment", comment}}});
    }
    fits_read_record(fptr, ++keynum, record, &status);
  }

  return headers;
}

void FitsFile::loadFitsHeader() {
  int status = 0;

  // save current HDU (if the file is opened with advanced cfitsio syntax it might be set already)
  int original_hdu = 0;
  fits_get_hdu_num(m_fits_ptr.get(), &original_hdu);

  int hdu_type = 0;
  for (unsigned int i = 0; i < m_headers.size(); i++) {
    fits_movabs_hdu(m_fits_ptr.get(), i + 1, &hdu_type, &status);  // +1 hdus start at 1

    m_headers[i] = loadHeadersFromFits(m_fits_ptr.get());
  }

  // go back to saved HDU
  fits_movabs_hdu(m_fits_ptr.get(), original_hdu, &hdu_type, &status);
}

void FitsFile::loadHeadFile() {
  auto base_name = m_path.stem();
  base_name.replace_extension(".head");
  auto head_filename = m_path.parent_path() / base_name;

  if (!boost::filesystem::exists(head_filename)) {
    return;
  }

  auto          hdu_iter = m_image_hdus.begin();
  std::ifstream file;

  // open the file and check
  file.open(head_filename.native());
  if (!file.good() || !file.is_open()) {
    throw Elements::Exception() << "Cannot load ascii header file: " << head_filename;
  }

  while (file.good() && hdu_iter != m_image_hdus.end()) {
    int current_hdu = *hdu_iter;

    std::string line;
    std::getline(file, line);

    static boost::regex regex_blank_line("\\s*$");
    line = boost::regex_replace(line, regex_blank_line, std::string(""));
    if (line.size() == 0) {
      continue;
    }

    if (boost::to_upper_copy(line) == "END") {
      current_hdu = *(++hdu_iter);
    } else {
      static boost::regex regex("([^=]{1,8})=([^\\/]*)(\\/ (.*))?");
      boost::smatch       sub_matches;
      if (boost::regex_match(line, sub_matches, regex) && sub_matches.size() >= 3) {
        auto keyword = boost::to_upper_copy(sub_matches[1].str());
        auto value   = sub_matches[2].str();
        auto comment = sub_matches[4].str();
        boost::trim(keyword);
        boost::trim(value);
        boost::trim(comment);
        m_headers.at(current_hdu - 1)[keyword] = MetadataEntry{valueAutoCast(value), {{"comment", comment}}};
        ;
      }
    }
  }
}

}  // namespace SourceXtractor
