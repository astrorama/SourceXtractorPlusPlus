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
    info.m_file_pointer = openFitsFile(filename, info.m_is_writeable);
    info.m_is_file_opened = true;
    m_open_files.push_front(filename);

    closeExtraFiles();
  }

  if (writeable && !info.m_is_writeable) {
    closeFitsFile(info.m_file_pointer);
    info.m_file_pointer = openFitsFile(filename, true);
    info.m_is_writeable = true;
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


fitsfile* FitsFileManager::openFitsFile(const std::string& filename, bool writeable) const {
  int status = 0;
  fitsfile* fptr =  nullptr;

  fits_open_file(&fptr, filename.c_str(), writeable ? READWRITE : READONLY, &status);
  if (status != 0) {
    throw Elements::Exception() << "Can't open FITS file: " << filename;
  }
  assert(fptr != nullptr);

  return fptr;
}

void FitsFileManager::closeFitsFile(fitsfile* fptr) const {
  int status = 0;
  fits_close_file(fptr, &status);
}

}
