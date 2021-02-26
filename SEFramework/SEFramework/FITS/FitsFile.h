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

#include "SEFramework/Image/ImageSourceWithMetadata.h"
#include "SEFramework/FITS/FitsFileManager.h"

namespace SourceXtractor {

/**
 * @class FitsFile
 * @brief represents access to a whole FITS file and handles loading and caching FITS headers
 * @details
 *  The implementation probably needs some explanation:
 *  In principle, the FitsFileManager may decide to close a file descriptor anytime
 *  it sees fit. It could be while someone is using the pointer this class holds (that's why
 *  getFitsFilePtr returns a std::shared_ptr, so it is kept alive while needed). It could be
 *  just between opening a file and returning it. The former has a non negligible chance,
 *  the later would be extremely unlikely (and unlucky), but is a race condition nevertheless.
 *  That's why doOpen returns a shared_ptr apart of setting the m_file_pointer member. This should
 *  (I think) solve the issue, as there will be a shared_ptr keeping the file descriptor alive
 *  all the way to the caller of getFitsFilePtr(), even if close() is called in the middle.
 * @warning
 *  The FitsFileManager must outlive any instance of FitsFile it created.
 *  We do not store a shared pointer because otherwise there is a reference cycle between the
 *  FitsManager and the FitsFile
 */
class FitsFile {
protected:
  FitsFile(const std::string& filename, bool writeable, FitsFileManager *manager);

public:

  virtual ~FitsFile();

  /**
   * @return A shared pointer to a low-level fitsfile handler
   * This point should *not* be stored anywhere. It is return as a std::shared_ptr
   * so it is guaranteed to stay alive as long as it is needed, even if the FitsFileManager
   * decides to close it on a separate thread
   */
  std::shared_ptr<fitsfile> getFitsFilePtr();

  const std::vector<int>& getImageHdus() const {
    return m_image_hdus;
  }

  std::map<std::string, MetadataEntry>& getHDUHeaders(int hdu) {
    return m_headers.at(hdu-1);
  }

  const std::string& getFilename() const {
    return m_filename;
  }

  bool isOpen() const;

  void setWriteMode();

  void open();

  void close();

private:
  friend class FitsFileManager;

  std::string m_filename;
  std::shared_ptr<fitsfile> m_file_pointer;
  bool m_is_writeable;
  bool m_was_opened_before;

  std::vector<int> m_image_hdus;
  std::vector<std::map<std::string, MetadataEntry>> m_headers;
  FitsFileManager* m_manager;

  std::shared_ptr<fitsfile> doOpen();
  fitsfile* openFirstTime();
  fitsfile* reopen();

  void reloadHeaders(fitsfile* fptr);
  std::map<std::string, MetadataEntry> loadFitsHeader(fitsfile *fptr);
  void loadHeadFile();
};

}

#endif /* _SEFRAMEWORK_FITS_FITSFILE_H_ */
