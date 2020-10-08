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

#include "SEUtils/VariantCast.h"

#include <SEImplementation/PythonConfig/PyFitsFile.h>

namespace SourceXtractor {

PyFitsFile::PyFitsFile(const std::string& filename) : m_filename(filename) {
  m_file = FitsFileManager::getInstance()->getFitsFile(filename);
  m_file->open();
}

std::vector<int> PyFitsFile::getImageHdus() const {
  std::vector<int> v;

  for(auto num : m_file->getImageHdus()) {
    v.push_back(num-1);
  }

  return v;
}

std::map<std::string, std::string> PyFitsFile::getHeaders(int hdu) const {
  const auto& headers = m_file->getHDUHeaders(hdu+1);
  std::map<std::string, std::string> headers_str;

  for (auto& i : headers) {
    headers_str[i.first] = VariantCast<std::string>(i.second.m_value);
  }

  return headers_str;
}

}
