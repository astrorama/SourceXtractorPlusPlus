#include "PythonConfig/PyAperture.h"


namespace SExtractor {

namespace bp = boost::python;

PyAperture::PyAperture(const boost::python::list &py_apertures) {
  for (int i = 0; i < bp::len(py_apertures); ++i) {
    apertures.push_back(bp::extract<float>(py_apertures[i]));
  }
}

std::string PyAperture::toString() const {
  std::stringstream str;
  str << "(ID:" << id << ", apertures:[";
  for (int i = 0; i < apertures.size(); ++i) {
    str << apertures[i];
    if (i < apertures.size() - 1) {
      str << ",";
    }
  }
  str << "])";
  return str.str();
}

} // end SExtractor
