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

#include <boost/filesystem/path.hpp>
#include <fitsio.h>
#include <map>
#include <string>
#include <vector>

#include "SEFramework/Image/ImageSourceWithMetadata.h"

namespace SourceXtractor {

/**
 * @class FitsFile
 * @brief represents access to a whole FITS file and handles loading and caching FITS headers
 *
 */
class FitsFile {
public:
  FitsFile(const boost::filesystem::path& path, bool writeable);

  FitsFile(FitsFile&&) = default;

  virtual ~FitsFile();

  fitsfile* getFitsFilePtr();

  const std::vector<int>& getImageHdus() const;

  std::map<std::string, MetadataEntry>& getHDUHeaders(int hdu);
  std::vector<int> getDimensions(int hdu) const;

  void refresh();

private:
  boost::filesystem::path m_path;
  bool m_is_writeable;
  std::unique_ptr<fitsfile, void (*)(fitsfile*)> m_fits_ptr;
  std::vector<int> m_image_hdus;
  std::vector<std::map<std::string, MetadataEntry>> m_headers;

  void open();
  void loadInfo();
  void loadFitsHeader();
  void loadHeadFile();
};

}  // namespace SourceXtractor

#endif /* _SEFRAMEWORK_FITS_FITSFILE_H_ */
