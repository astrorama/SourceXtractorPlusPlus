/**
 * @file SEUtils/Python.h
 * @date 16/11/2018
 * @author Alejandro Alvarez Ayllon
 */

#ifndef _SEUTILS_PYTHON_H
#define _SEUTILS_PYTHON_H

#include <ElementsKernel/Exception.h>
#include <ElementsKernel/Logging.h>

namespace SExtractor {
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

} // end SExtractor

#endif // _SEUTILS_PYTHON_H
