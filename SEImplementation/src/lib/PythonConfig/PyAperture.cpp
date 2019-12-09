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

#include "SEImplementation/PythonConfig/PyAperture.h"
#include <boost/python/extract.hpp>


namespace SourceXtractor {

namespace bp = boost::python;

PyAperture::PyAperture(const boost::python::list &py_apertures) {
  for (int i = 0; i < bp::len(py_apertures); ++i) {
    apertures.push_back(bp::extract<float>(py_apertures[i]));
  }
}

std::string PyAperture::toString() const {
  std::stringstream str;
  str << "(ID:" << id << ", apertures:[";
  for (unsigned int i = 0; i < apertures.size(); ++i) {
    str << apertures[i];
    if (i < apertures.size() - 1) {
      str << ",";
    }
  }
  str << "])";
  return str.str();
}

} // end SourceXtractor
