/*
 * ProgressPrinter.h
 *
 *  Created on: Mar 19, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSPRINTER_H
#define _SEMAIN_PROGRESSPRINTER_H

#include <map>
#include <string>

namespace SExtractor {

class ProgressPrinter {
public:
  virtual ~ProgressPrinter() = default;

  ProgressPrinter() = default;

  void update(const std::string& label, int count, int total = -1) {
    m_progress_info[label] = std::make_pair(count, total);
  }

  virtual void print() = 0;
  virtual void done() {
    m_done = true;
  }

protected:
  std::map<std::string, std::pair<int, int>> m_progress_info;
  bool m_done = false;
};

} // end SExtractor

#endif // _SEMAIN_PROGRESSPRINTER_H
