/*
 * @file PyAperture.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _SEIMPLEMENTATION_PYAPERTURE_H
#define _SEIMPLEMENTATION_PYAPERTURE_H

#include <vector>
#include <boost/python.hpp>
#include "SEImplementation/PythonConfig/PyId.h"

namespace SExtractor {


class PyAperture : public PyId {
public:
  PyAperture(const boost::python::list &py_apertures);

  std::vector<float> apertures;

  std::string toString() const;
};

}

#endif // _SEIMPLEMENTATION_PYMEASUREMENTIMAGE_H

