/*
 * @file PyColumn.h
 * @author Alejandro Alvarez Ayllon
 */

#ifndef _PYTHONCONFIG_PYCOLUMN_H
#define _PYTHONCONFIG_PYCOLUMN_H

#include <atomic>
#include <map>

namespace SExtractor {

class PyColumn {
public:
  PyColumn();
  virtual ~PyColumn();

  const int id;

private:
  static int s_next_col_id;
};

}

#endif // _PYTHONCONFIG_PYMEASUREMENTIMAGE_H

