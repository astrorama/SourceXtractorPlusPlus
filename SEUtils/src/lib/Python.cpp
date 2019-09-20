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
#include <Python.h>
#include <ElementsKernel/Logging.h>
#include <boost/python.hpp>

#include "SEUtils/Python.h"


namespace py = boost::python;

namespace SExtractor {

Elements::Exception pyToElementsException(Elements::Logging &logger) {
  GILStateEnsure ensure;

  PyObject *ptype, *pvalue, *ptraceback;
  PyErr_Fetch(&ptype, &pvalue, &ptraceback);
  PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);

  py::handle<> handle_type(ptype);
  py::handle<> handle_value(pvalue);
  py::handle<> handle_traceback(py::allow_null(ptraceback));

  // Get only the error message
  py::object
  err_msg_obj(py::handle<>(PyObject_Str(pvalue)));
  std::string err_msg = py::extract<std::string>(err_msg_obj);

  // Dump to ERR the traceback
  logger.error() << "Python exception " << err_msg;

  if (ptraceback) {
    py::object traceback(handle_traceback);
    while (traceback) {
      long lineno = py::extract<long>(traceback.attr("tb_lineno"));
      std::string filename = py::extract<std::string>(traceback.attr("tb_frame").attr("f_code").attr("co_filename"));
      std::string funcname = py::extract<std::string>(traceback.attr("tb_frame").attr("f_code").attr("co_name"));

      logger.error() << "File \"" << filename << "\", line " << lineno << ", in " << funcname;

      traceback = traceback.attr("tb_next");
    }
  }

  // Done
  PyErr_Clear();
  return Elements::Exception(err_msg);
}

GILStateEnsure::GILStateEnsure() {
  m_state = PyGILState_Ensure();
}

GILStateEnsure::~GILStateEnsure() {
  PyGILState_Release(m_state);
}

} // end SExtractor
