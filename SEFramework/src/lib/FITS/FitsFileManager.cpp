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

#include "SEFramework/FITS/FitsFile.h"

#include "SEFramework/FITS/FitsFileManager.h"

namespace SourceXtractor {

std::shared_ptr<FitsFileManager> FitsFileManager::s_instance;

FitsFileManager::FitsFileManager(unsigned int max_open_files) : m_max_open_files(max_open_files) {
}

FitsFileManager::~FitsFileManager() {
  closeAllFiles();
}

void FitsFileManager::closeAllFiles() {
  for (auto& file : m_fits_files) {
    file.second->close();
  }
}

std::shared_ptr<FitsFile> FitsFileManager::getFitsFile(const std::string& filename, bool writeable) {
  if (m_fits_files.find(filename) != m_fits_files.end()) {
    auto fits_file = m_fits_files.at(filename);
    if (writeable) {
      fits_file->setWriteMode();
    }

    return fits_file;
  } else {
    auto new_fits_file = std::shared_ptr<FitsFile>(new FitsFile(filename, writeable, shared_from_this()));
    m_fits_files[filename] = new_fits_file;
    return new_fits_file;
  }
}


void FitsFileManager::closeExtraFiles() {
  while (m_open_files.size() > m_max_open_files) {
    auto& file_to_close = m_fits_files[m_open_files.back()];
    file_to_close->close();
  }
}

void FitsFileManager::reportClosedFile(const std::string& filename) {
  m_open_files.remove(filename);
}

void FitsFileManager::reportOpenedFile(const std::string& filename) {
  auto iter = std::find(m_open_files.begin(), m_open_files.end(), filename);
  if (iter == m_open_files.end()) {
    m_open_files.push_front(filename);
  }
  closeExtraFiles();
}


}
