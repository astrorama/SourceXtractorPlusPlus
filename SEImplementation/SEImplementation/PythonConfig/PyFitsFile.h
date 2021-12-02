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

#ifndef _SEIMPLEMENTATION_PYFITSFILE_H
#define _SEIMPLEMENTATION_PYFITSFILE_H

#include <string>
#include <boost/python/list.hpp>
#include "SEFramework/FITS/FitsFile.h"
#include "SEImplementation/PythonConfig/PyId.h"

namespace SourceXtractor {

class PyFitsFile {

public:

  explicit PyFitsFile(const std::string& filename);

  virtual ~PyFitsFile() = default;

  std::string getFilename() const {
    return m_filename;
  }

  std::vector<int> getImageHdus() const;

  std::map<std::string, std::string> getHeaders(int hdu) const;

private:
  std::string m_filename;
  std::shared_ptr<FitsFile> m_file;
};

}

#endif // _SEIMPLEMENTATION_PYFITSFILE_H

