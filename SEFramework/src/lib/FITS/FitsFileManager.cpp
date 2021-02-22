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
#include <algorithm>

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
  std::lock_guard<std::mutex> lock(m_mutex);

  for (auto& file : m_fits_files) {
    for (auto& fd : file.second) {
      fd->close();
    }
  }
}

std::shared_ptr<FitsFile> FitsFileManager::getFitsFile(const std::string& filename, bool writeable) {
  std::lock_guard<std::mutex> lock(m_mutex);

  auto it = m_fits_files.find(filename);
  if (it == m_fits_files.end()) {
    it = m_fits_files.emplace(filename, std::deque<std::unique_ptr<FitsFile>>{}).first;
  }

  auto& pool = it->second;
  if (pool.empty()) {
    pool.push_back(std::unique_ptr<FitsFile>(new FitsFile(filename, writeable)));
  }

  auto fits_ptr = pool.front().release();
  pool.pop_front();
  std::shared_ptr<FitsFile> fits_file(fits_ptr, [this](FitsFile *fits_ptr) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (fits_ptr->isOpen()) {
      this->m_fits_files[fits_ptr->getFilename()].push_back(std::unique_ptr<FitsFile>(fits_ptr));
      closeExtraFiles();
    }
    else {
      delete fits_ptr;
    }
  });
  if (writeable) {
    fits_file->setWriteMode();
  }
  return fits_file;
}


void FitsFileManager::closeExtraFiles() {
  unsigned opened = 0;
  for (auto& pool : m_fits_files) {
    opened += pool.second.size();
  }

  auto i = m_fits_files.begin();
  while (opened > m_max_open_files) {
    i->second.pop_front();
    --opened;
    ++i;
    if (i == m_fits_files.end()) {
      i = m_fits_files.begin();
    }
  }
}

unsigned FitsFileManager::count() const {
  std::lock_guard<std::mutex> lock(m_mutex);

  unsigned opened = 0;
  for (auto& pool : m_fits_files) {
    opened += pool.second.size();
  }
  return opened;
}

void FitsFileManager::closeAndPurgeFile(const std::string& filename) {
  std::lock_guard<std::mutex> lock(m_mutex);

  auto it = m_fits_files.find(filename);
  if (it != m_fits_files.end()) {
    for (auto& fd : it->second) {
      fd->close();
    }
    m_fits_files.erase(filename);
  }
}

}
