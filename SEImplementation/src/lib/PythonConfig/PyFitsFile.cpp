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
#include <boost/algorithm/string.hpp>

namespace SourceXtractor {

PyFitsFile::PyFitsFile(const std::string& filename) : m_filename(filename) {
  m_file = std::make_shared<FitsFile>(filename, false);
}

std::vector<int> PyFitsFile::getImageHdus() const {
  std::vector<int> v;

  for(auto num : m_file->getImageHdus()) {
    v.push_back(num-1);
  }

  return v;
}

std::vector<int> PyFitsFile::getDimensions(int hdu) const {
  return m_file->getDimensions(hdu+1);
}


std::map<std::string, std::string> PyFitsFile::getHeaders(int hdu) const {
  const auto& headers = m_file->getHDUHeaders(hdu+1);
  std::map<std::string, std::string> headers_str;

  for (auto& i : headers) {
    auto key = i.first;
    auto value = VariantCast<std::string>(i.second.m_value);
    boost::trim(key);
    boost::trim(value);
    headers_str[key] = value;
  }

  return headers_str;
}

}
