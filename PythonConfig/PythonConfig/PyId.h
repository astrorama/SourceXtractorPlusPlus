/*
 * @file PyId.h
 * @author Alejandro Alvarez Ayllon
 */

#ifndef _PYTHONCONFIG_PYID_H
#define _PYTHONCONFIG_PYID_H

#include <atomic>
#include <map>

namespace SExtractor {

class PyId {
public:
  PyId();
  virtual ~PyId();

  const int id;

private:
  static int s_next_col_id;
};

}

#endif // _PYTHONCONFIG_PYID_H

