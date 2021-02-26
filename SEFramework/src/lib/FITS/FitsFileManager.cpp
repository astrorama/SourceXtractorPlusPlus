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
  // There will be callbacks! So operate over a separate container
  std::list<FitsFile *> to_close;
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& pool : m_available_files) {
      for (auto& fd : pool.second) {
        to_close.emplace_back(fd.get());
      }
    }
    for (auto& open : m_opened_files) {
      to_close.emplace_back(open);
    }
  }

  for (auto& fd : to_close) {
    fd->close();
  }

  std::lock_guard<std::mutex> lock(m_mutex);
  m_available_files.clear();
}

std::shared_ptr<FitsFile>
FitsFileManager::getFitsFile(const std::string& filename, bool writeable) {
  std::lock_guard<std::mutex> lock(m_mutex);

  auto it = m_available_files.find(filename);
  if (it == m_available_files.end()) {
    it = m_available_files.emplace(filename, std::deque<std::unique_ptr<FitsFile>>{}).first;
  }

  auto& pool = it->second;
  if (pool.empty()) {
    pool.push_back(std::unique_ptr<FitsFile>(new FitsFile(filename, writeable, this)));
  }

  auto fits_ptr = pool.front().release();
  pool.pop_front();
  // Return a shared_ptr with a custom destructor that returns the file to the pool
  std::shared_ptr<FitsFile> fits_file(fits_ptr, [this](FitsFile *fits_ptr) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (fits_ptr->isOpen()) {
      this->m_available_files[fits_ptr->getFilename()].push_back(
        std::unique_ptr<FitsFile>(fits_ptr));
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
  std::unique_lock<std::mutex> lock(m_mutex);

  unsigned owned_count = 0;
  for (auto& pool : m_available_files) {
    owned_count += pool.second.size();
  }
  unsigned tracked_count = m_opened_files.size();

  // Start by purging unused files
  auto oi = m_available_files.begin();
  while ((owned_count + tracked_count) > m_max_open_files && owned_count) {
    oi->second.pop_front();
    --owned_count;
    ++oi;
    if (oi == m_available_files.end()) {
      oi = m_available_files.begin();
    }
  }

  // Close used files if we need
  std::list<FitsFile *> to_close;
  for (auto ti = m_opened_files.begin(); ti != m_opened_files.end() && tracked_count > m_max_open_files; ++ti) {
    to_close.emplace_back(*ti);
    --tracked_count;
  }

  // Beware! FitsFile will report back, so release the lock for this part
  lock.unlock();
  for (auto& ptr : to_close) {
    ptr->close();
  }
}

void FitsFileManager::reportClosedFile(FitsFile *fits_file) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_opened_files.erase(fits_file);
}

void FitsFileManager::closeAndPurgeFile(const std::string& filename) {
  std::lock_guard<std::mutex> lock(m_mutex);

  // Owned
  auto it = m_available_files.find(filename);
  if (it != m_available_files.end()) {
    for (auto& fd : it->second) {
      fd->close();
    }
    m_available_files.erase(filename);
  }

  // Tracked
  for (auto ti = m_opened_files.begin(); ti != m_opened_files.end();) {
    if ((*ti)->getFilename() == filename) {
      (*ti)->close();
      ti = m_opened_files.erase(ti);
    }
    else {
      ++ti;
    }
  }
}

void FitsFileManager::reportOpenedFile(FitsFile *fits_file) {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_opened_files.emplace(fits_file);
  }
  closeExtraFiles();
}

}
