#include "SEUtils/Python.h"

#include <ElementsKernel/Logging.h>
#include <Python.h>
#include <boost/python.hpp>

namespace py = boost::python;

namespace SExtractor {

Elements::Exception pyToElementsException(Elements::Logging &logger) {
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

} // end SExtractor
