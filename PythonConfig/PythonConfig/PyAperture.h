/*
 * @file PyAperture.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _PYTHONCONFIG_PYAPERTURE_H
#define _PYTHONCONFIG_PYAPERTURE_H

#include <vector>
#include <boost/python.hpp>
#include "PythonConfig/PyColumn.h"

namespace SExtractor {


class PyAperture : public PyColumn {
public:
  PyAperture(const boost::python::list &py_apertures);

  std::vector<float> apertures;

  std::string toString() const;
};

}

#endif // _PYTHONCONFIG_PYMEASUREMENTIMAGE_H

