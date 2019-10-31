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
/**
 * @file SEUtils/Python.h
 * @date 16/11/2018
 * @author Alejandro Alvarez Ayllon
 */

#ifndef _SEUTILS_PYTHON_H
#define _SEUTILS_PYTHON_H

#include <ElementsKernel/Exception.h>
#include <ElementsKernel/Logging.h>
#include <Python.h>

namespace SourceXtractor {
/**
 * Dumps into ERR the traceback and returns an Elements exception to be re-thrown
 * @param logger
 *  Dumps the traceback to this logger
 * @note
 *  It uses PyErr_Fetch to get the exception
 * @return
 *  An Elements exception that contains the error message from the last exception thrown within Python
 */
Elements::Exception pyToElementsException(Elements::Logging &logger);

/**
 * RAII wrapper to be used when going back to Python
 */
struct GILStateEnsure {
public:
  GILStateEnsure();
  ~GILStateEnsure();

private:
  PyGILState_STATE m_state;
};

} // end SourceXtractor

#endif // _SEUTILS_PYTHON_H
