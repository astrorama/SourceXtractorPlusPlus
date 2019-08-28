/*
 * @file PyOutputWrapper.cpp
 * @author Alejandro Alvarez Ayllon
 */

#include <boost/python.hpp>
#include "SEImplementation/PythonConfig/PyOutputWrapper.h"

namespace SExtractor {

namespace bp = boost::python;

PyOutputWrapper::PyOutputWrapper(Elements::Logging& logger) : closed(false), m_logger(logger) {}

void PyOutputWrapper::close() {
  PyErr_Format(PyExc_OSError, "Method %s not supported on wrapper file objects", __func__);
  bp::throw_error_already_set();
}

int PyOutputWrapper::fileno() const {
  PyErr_Format(PyExc_OSError, "Method %s not supported on wrapper file objects", __func__);
  bp::throw_error_already_set();
  return -1;
}

void PyOutputWrapper::flush() {
}

bool PyOutputWrapper::isatty() const {
  return false;
}

bool PyOutputWrapper::readable() const {
  return false;
}

std::string PyOutputWrapper::read(int) {
  PyErr_Format(PyExc_IOError, "Object is write only");
  bp::throw_error_already_set();
  return {};
}

std::string PyOutputWrapper::readline(int) {
  PyErr_Format(PyExc_IOError, "Object is write only");
  bp::throw_error_already_set();
  return {};
}

bp::list PyOutputWrapper::readlines(int) {
  PyErr_Format(PyExc_IOError, "Object is write only");
  bp::throw_error_already_set();
  return {};
}

int PyOutputWrapper::seek(int, int) {
  PyErr_Format(PyExc_OSError, "Method %s not supported on wrapper file objects", __func__);
  bp::throw_error_already_set();
  return 0;
}

bool PyOutputWrapper::seekable() const {
  return false;
}

int PyOutputWrapper::tell() const {
  PyErr_Format(PyExc_OSError, "Method %s not supported on wrapper file objects", __func__);
  bp::throw_error_already_set();
  return 0;
}

void PyOutputWrapper::truncate(int) {
  PyErr_Format(PyExc_OSError, "Method %s not supported on wrapper file objects", __func__);
  bp::throw_error_already_set();
}

bool PyOutputWrapper::writable() const {
  return true;
}

int PyOutputWrapper::write(const std::string& str) {
  for (auto c : str) {
    if (c == '\n') {
      m_logger.info() << m_buffer.str();
      m_buffer.str(std::string());
    } else {
      m_buffer << c;
    }
  }
  return str.size();
}

void PyOutputWrapper::writelines(const boost::python::list& lines) {
  for (int i = 0; i < bp::len(lines); ++i) {
    std::string line = bp::extract<std::string>(lines[i]);
    m_logger.info() << line;
  }
}

} // end namespace SExtractor

