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
