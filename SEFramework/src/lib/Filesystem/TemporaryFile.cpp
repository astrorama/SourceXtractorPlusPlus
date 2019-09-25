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
 * TemporaryFile.cpp
 *
 *  Created on: May 23, 2018
 *      Author: aalvarez
 */

#include "SEFramework/Filesystem/TemporaryFile.h"
#include <boost/filesystem.hpp>

namespace SExtractor {

static std::string generateTemporaryPath(const std::string& dir, const std::string& pattern) {
  boost::filesystem::path tmp_path;
  boost::filesystem::path file_name;

  if (pattern.empty()) {
    file_name = boost::filesystem::unique_path();
  }
  else {
    file_name = boost::filesystem::unique_path(pattern);
  }
  if (dir.empty()) {
    tmp_path = boost::filesystem::temp_directory_path();
  }
  else {
    tmp_path = dir;
  }

  return (tmp_path / file_name).native();
}


TemporaryFile::TemporaryFile(bool autoremove): TemporaryFile("", "", autoremove) {
}


TemporaryFile::TemporaryFile(const std::string &pattern, bool autoremove): TemporaryFile("", pattern, autoremove) {
}


TemporaryFile::TemporaryFile(const std::string &dir, const std::string &pattern, bool autoremove)
    : m_path(generateTemporaryPath(dir, pattern)), m_autoremove(autoremove) {
}


TemporaryFile::~TemporaryFile() {
  if (m_autoremove && boost::filesystem::exists(m_path)) {
    boost::filesystem::remove(m_path);
  }
}

const std::string& TemporaryFile::getPath() const {
  return m_path;
}

}
