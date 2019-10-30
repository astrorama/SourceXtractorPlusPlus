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
 * TemporaryFile.h
 *
 *  Created on: May 23, 2018
 *      Author: aalvarez
 */

#ifndef _SEFRAMEWORK_FILESYSTEM_TEMPORARYFILE_H
#define _SEFRAMEWORK_FILESYSTEM_TEMPORARYFILE_H

#include <string>

namespace SourceXtractor {

class TemporaryFile {
public:
  TemporaryFile(bool autoremove = true);
  TemporaryFile(const std::string &pattern, bool autoremove = true);
  TemporaryFile(const std::string &dir, const std::string &pattern, bool autoremove = true);

  ~TemporaryFile();

  /// Return the full path of the temporary file
  const std::string& getPath() const;

private:
  std::string m_path;
  bool m_autoremove;
};

}

#endif //_SEFRAMEWORK_FILESYSTEM_TEMPORARYFILE_H
