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

#include <iomanip>
#include <fstream>
#include <string>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "ElementsKernel/Exception.h"

#include "SEFramework/FITS/FitsFile.h"

namespace SourceXtractor {


FitsFile::FitsFile(const std::string& filename, bool writeable, std::shared_ptr<FitsFileManager> manager) :
      m_filename(filename),
      m_file_pointer(nullptr),
      m_is_file_opened(false),
      m_is_writeable(writeable),
      m_was_opened_before(false),
      m_manager(manager) {
}

FitsFile::~FitsFile() {
  close();
}

void FitsFile::openFirstTime() {
  int status = 0;

  fits_open_image(&m_file_pointer, m_filename.c_str(), m_is_writeable ? READWRITE : READONLY, &status);
  if (status != 0) {
    throw Elements::Exception() << "Can't open FITS file: " << m_filename;
  }

  m_image_hdus.clear() ;
  int number_of_hdus = 0;
  if (fits_get_num_hdus(m_file_pointer, &number_of_hdus, &status) < 0) {
    throw Elements::Exception() << "Can't get the number of HDUs in FITS file: " << m_filename;
  }

  m_headers.resize(number_of_hdus);

  // save current HDU (if the file is opened with advanced cfitsio syntax it might be set already
  int original_hdu = 0;
  fits_get_hdu_num(m_file_pointer, &original_hdu);

  // loop over HDUs to determine which ones are images
  int hdu_type = 0;
  for (int hdu_number=1; hdu_number <= number_of_hdus; hdu_number++) {
    fits_movabs_hdu(m_file_pointer, hdu_number, &hdu_type, &status);
    if (status != 0) {
      throw Elements::Exception() << "Can't switch HDUs while opening: " << m_filename;
    }

    if (hdu_type == IMAGE_HDU) {
      int bitpix, naxis;
      long naxes[2] = {1,1};

      fits_get_img_param(m_file_pointer, 2, &bitpix, &naxis, naxes, &status);
      if (status == 0 && naxis == 2) {
        m_image_hdus.emplace_back(hdu_number);
      }
    }
  }

  // go back to saved HDU
  fits_movabs_hdu(m_file_pointer, original_hdu, &hdu_type, &status);

  // load all FITS headers
  loadHeaders();

  // load optional .head file to override headers
  loadHeadFile();

  m_is_file_opened = true;
  m_was_opened_before = true;
}

void FitsFile::reopen() {
  int status = 0;
  fits_open_image(&m_file_pointer, m_filename.c_str(), m_is_writeable ? READWRITE : READONLY, &status);
  if (status != 0) {
    throw Elements::Exception() << "Can't open FITS file: " << m_filename;
  }
  m_is_file_opened = true;
}

void FitsFile::open() {
  if (!m_is_file_opened) {
    if (m_was_opened_before) {
      reopen();
    } else {
      openFirstTime();
    }
  }
  assert(m_file_pointer != nullptr);
}

void FitsFile::close() {
  if (m_is_file_opened) {
    int status = 0;
    fits_close_file(m_file_pointer, &status);
    m_file_pointer = nullptr;
    m_is_file_opened = false;
  }
}

void FitsFile::setWriteMode() {
  if (!m_is_writeable) {
    close();
    m_is_writeable = true;
    open();
  }
}

void FitsFile::loadHeaders() {
  int status = 0;

  // save current HDU (if the file is opened with advanced cfitsio syntax it might be set already)
  int original_hdu = 0;
  fits_get_hdu_num(m_file_pointer, &original_hdu);

  int hdu_type = 0;
  for (unsigned int i = 0; i < m_headers.size(); i++) {
    fits_movabs_hdu(m_file_pointer, i+1, &hdu_type, &status); // +1 hdus start at 1

    m_headers[i] = loadFitsHeader(m_file_pointer);
  }

  // go back to saved HDU
  fits_movabs_hdu(m_file_pointer, original_hdu, &hdu_type, &status);
}

std::map<std::string, std::string> FitsFile::loadFitsHeader(fitsfile *fptr) {
  std::map<std::string, std::string> headers;
  char record[81];
  int keynum = 1, status = 0;

  fits_read_record(fptr, keynum, record, &status);
  while (status == 0 && strncmp(record, "END", 3) != 0) {
    static boost::regex regex("([^=]{8})=([^\\/]*)(.*)");
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

void FitsFile::loadHeadFile() {
  auto filename = boost::filesystem::path(m_filename);
  auto base_name = filename.stem();
  base_name.replace_extension(".head");
  auto head_filename = filename.parent_path() / base_name;

  auto hdu_iter = m_image_hdus.begin();
  int current_hdu = *hdu_iter;

  if (boost::filesystem::exists(head_filename)) {
    std::ifstream file;

    // open the file and check
    file.open(head_filename.native());
    if (!file.good() || !file.is_open()) {
      throw Elements::Exception() << "Cannot load ascii header file: " << head_filename;
    }

    while (file.good() && hdu_iter != m_image_hdus.end()) {
      std::string line;
      std::getline(file, line);

      static boost::regex regex_blank_line("\\s*$");
      line = boost::regex_replace(line, regex_blank_line, std::string(""));
      if (line.size() == 0) {
        continue;
      }

      if (boost::to_upper_copy(line) == "END") {
        current_hdu = *(++hdu_iter);
      }
      else {
        static boost::regex regex("([^=]{1,8})=([^\\/]*)(.*)");
        boost::smatch sub_matches;
        if (boost::regex_match(line, sub_matches, regex) && sub_matches.size() >= 3) {
          auto keyword = boost::to_upper_copy(sub_matches[1].str());
          auto value = sub_matches[2].str();
          boost::trim(keyword);
          boost::trim(value);
          m_headers.at(current_hdu-1)[keyword] = value;
        }
      }
    }
  }
}


}
