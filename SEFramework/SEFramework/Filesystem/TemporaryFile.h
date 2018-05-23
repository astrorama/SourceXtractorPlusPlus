/*
 * TemporaryFile.h
 *
 *  Created on: May 23, 2018
 *      Author: aalvarez
 */

#ifndef _SEFRAMEWORK_FILESYSTEM_TEMPORARYFILE_H
#define _SEFRAMEWORK_FILESYSTEM_TEMPORARYFILE_H

#include <string>

namespace SExtractor {

class TemporaryFile {
public:
  TemporaryFile(bool autoremove = true);
  TemporaryFile(const std::string &pattern, bool autoremove = true);
  TemporaryFile(const std::string &dir, const std::string &pattern, bool autoremove = true);

  ~TemporaryFile();

  /// Return the full path of the temporary file
  const std::string& getPath() const;

  /// Create the file on disk
  /// @return An output stream pointing to the file
  std::ofstream touch() const;

private:
  std::string m_path;
  bool m_autoremove;
};

}

#endif //_SEFRAMEWORK_FILESYSTEM_TEMPORARYFILE_H
