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
 * FitsFileManager.h
 *
 *  Created on: Sep 19, 2019
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_FITS_FITSFILEMANAGER_H_
#define _SEFRAMEWORK_FITS_FITSFILEMANAGER_H_

#include <memory>
#include <string>
#include <list>
#include <vector>
#include <deque>
#include <unordered_map>
#include <mutex>

#include <fitsio.h>

namespace SourceXtractor {

class FitsFile;

class FitsFileManager : public std::enable_shared_from_this<FitsFileManager> {
public:

  FitsFileManager(unsigned int max_open_files = 500);
  virtual ~FitsFileManager();

  void closeAllFiles();

  static std::shared_ptr<FitsFileManager> getInstance() {
    if (s_instance == nullptr) {
      s_instance = std::make_shared<FitsFileManager>();
    }
    return s_instance;
  }

  std::shared_ptr<FitsFile> getFitsFile(const std::string& filename, bool writeable=false);

  void closeAndPurgeFile(const std::string& filename);

  unsigned count() const;

private:
  mutable std::mutex m_mutex;
  std::unordered_map<std::string, std::deque<std::unique_ptr<FitsFile>>> m_fits_files;

  unsigned int m_max_open_files;

  static std::shared_ptr<FitsFileManager> s_instance;

  void closeExtraFiles();
};

}

#endif /* _SEFRAMEWORK_FITS_FITSFILEMANAGER_H_ */
