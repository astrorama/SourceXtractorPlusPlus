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
 * FitsFileManager.cpp
 *
 *  Created on: Sep 19, 2019
 *      Author: mschefer
 */

#include <iostream>
#include <assert.h>

#include "ElementsKernel/Exception.h"

#include "SEFramework/FITS/FitsFileManager.h"

namespace SourceXtractor {

std::shared_ptr<FitsFileManager> FitsFileManager::s_instance;

FitsFileManager::FitsFileManager() : m_max_open_files(500) {
}

FitsFileManager::~FitsFileManager() {
  closeAllFiles();
}

void FitsFileManager::closeAllFiles() {
  for (auto& file : m_fits_files) {
    if (file.second.m_is_file_opened) {
      closeFitsFile(file.second.m_file_pointer);
      file.second.m_is_file_opened = false;
      file.second.m_file_pointer = nullptr;
    }
  }
}


fitsfile* FitsFileManager::getFitsFile(const std::string& filename, bool writeable) {
  if (m_fits_files.find(filename) == m_fits_files.end()) {
    FitsInfo info;
    info.m_is_file_opened = false;
    info.m_file_pointer = nullptr;
    info.m_is_writeable = writeable;
    m_fits_files[filename] = info;
  }

  FitsInfo& info = m_fits_files[filename];
  if (!info.m_is_file_opened) {
    info.m_is_writeable = info.m_is_writeable || writeable;
    openFitsFile(filename, info);
    m_open_files.push_front(filename);

    closeExtraFiles();
  }

  if (writeable && !info.m_is_writeable) {
    closeFitsFile(info.m_file_pointer);
    info.m_is_writeable = true;
    openFitsFile(filename, info);
  }

  return info.m_file_pointer;
}

void FitsFileManager::closeExtraFiles() {
  while (m_open_files.size() > m_max_open_files) {
    auto& file_to_close = m_fits_files[m_open_files.back()];
    closeFitsFile(file_to_close.m_file_pointer);
    file_to_close.m_is_file_opened = false;
    file_to_close.m_file_pointer = nullptr;
    m_open_files.pop_back();
  }
}


void FitsFileManager::openFitsFile(const std::string& filename, FitsInfo& fits_info) const {
  if (fits_info.m_is_file_opened) {
    return;
  }

  int status = 0;

  fits_open_image(&fits_info.m_file_pointer, filename.c_str(), fits_info.m_is_writeable ? READWRITE : READONLY, &status);
  if (status != 0) {
    throw Elements::Exception() << "Can't open FITS file: " << filename;
  }
  assert(fits_info.m_file_pointer != nullptr);

  fits_info.m_is_file_opened = true;

  if (fits_info.m_image_hdus.empty()) {
    int number_of_hdus = 0;
    if (fits_get_num_hdus(fits_info.m_file_pointer, &number_of_hdus, &status) < 0) {
      throw Elements::Exception() << "Can't get the number of HDUs in FITS file: " << filename;
    }

    for (int hdu_number=1; hdu_number <= number_of_hdus; hdu_number++) {
      int hdu_type = 0;
      fits_movabs_hdu(fits_info.m_file_pointer, hdu_number, &hdu_type, &status);
      if (status != 0) {
        throw Elements::Exception() << "Can't switch HDUs while opening: " << filename;
      }

      if (hdu_type == IMAGE_HDU) {
        fits_info.m_image_hdus.emplace_back(hdu_number);
      }
    }
  }
}

void FitsFileManager::closeFitsFile(fitsfile* fptr) const {
  int status = 0;
  fits_close_file(fptr, &status);
}

}
