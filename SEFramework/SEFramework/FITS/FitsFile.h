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
 * FitsFile.h
 *
 *  Created on: Jun 9, 2020
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_FITS_FITSFILE_H_
#define _SEFRAMEWORK_FITS_FITSFILE_H_

#include <string>
#include <vector>
#include <map>

#include <fitsio.h>

#include "SEFramework/FITS/FitsFileManager.h"

namespace SourceXtractor {

/**
 * @class FitsFile
 * @brief represents access to a whole FITS file and handles loading and caching FITS headers
 *
 */
class FitsFile {
protected:
  FitsFile(const std::string& filename, bool writeable, std::shared_ptr<FitsFileManager> manager);

public:

  virtual ~FitsFile();

  fitsfile* getFitsFilePtr() {
    if (!m_is_file_opened) {
      open();
    }
    return m_file_pointer;
  }

  const std::vector<int>& getImageHdus() const {
    return m_image_hdus;
  }

  const std::map<std::string, std::string>& getHDUHeaders(int hdu) const {
    return m_headers.at(hdu-1);
  }

  void setWriteMode();

  void open();
  void close();

private:
  void openFirstTime();
  void reopen();

  std::map<std::string, std::string> loadFitsHeader(fitsfile *fptr);
  void loadHeaders();
  void loadHeadFile();

  std::string m_filename;
  fitsfile* m_file_pointer;
  bool m_is_file_opened;
  bool m_is_writeable;
  bool m_was_opened_before;

  std::vector<int> m_image_hdus;

  std::vector<std::map<std::string, std::string>> m_headers;

  std::shared_ptr<FitsFileManager> m_manager;

  friend class FitsFileManager;
};

}

#endif /* _SEFRAMEWORK_FITS_FITSFILE_H_ */
