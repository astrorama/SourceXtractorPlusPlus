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
