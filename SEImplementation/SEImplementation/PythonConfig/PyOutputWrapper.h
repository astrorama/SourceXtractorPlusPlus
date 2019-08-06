/*
 * @file PyOutputWrapper.h
 * @author Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PYOUTPUTWRAPPER_H
#define _SEIMPLEMENTATION_PYOUTPUTWRAPPER_H

#include <strstream>
#include <ElementsKernel/Logging.h>
#include <boost/python.hpp>

namespace SExtractor {

/**
 * File-like object used to redirect stdout and stderr from Python into
 * Elements' logger
 */
class PyOutputWrapper {
public:

  PyOutputWrapper(Elements::Logging &logger);

  // Can not be closed, but expose the API
  const bool closed;
  void close();

  // Dummy implementations: do nothing except either ignore or raise an exception
  int fileno() const;
  void flush();
  bool isatty() const;
  bool readable() const;
  std::string read(int);
  std::string readline(int);
  boost::python::list readlines(int);
  int seek(int, int);
  bool seekable() const;
  int tell() const;
  void truncate(int);

  // These do something!
  bool writable() const;
  int write(const std::string&);
  void writelines(const boost::python::list&);

private:
  Elements::Logging m_logger;
  std::ostringstream m_buffer;
};

} // end namespace SExtractor

#endif // _SEIMPLEMENTATION_PYOUTPUTWRAPPER_H
