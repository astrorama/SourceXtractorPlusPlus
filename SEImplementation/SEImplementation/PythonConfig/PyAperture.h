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
 * @file PyAperture.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _SEIMPLEMENTATION_PYAPERTURE_H
#define _SEIMPLEMENTATION_PYAPERTURE_H

#include <vector>
#include <boost/python.hpp>
#include "SEImplementation/PythonConfig/PyId.h"

namespace SourceXtractor {


class PyAperture : public PyId {
public:
  PyAperture(const boost::python::list &py_apertures);

  std::vector<float> apertures;

  std::string toString() const;
};

}

#endif // _SEIMPLEMENTATION_PYMEASUREMENTIMAGE_H

