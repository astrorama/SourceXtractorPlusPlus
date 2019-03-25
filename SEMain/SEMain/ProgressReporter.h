/*
 * ProgressReporter.h
 *
 *  Created on: Mar 19, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSPRINTER_H
#define _SEMAIN_PROGRESSPRINTER_H

#include <map>
#include <string>
#include "SEUtils/Observable.h"

namespace SExtractor {

typedef Observer<std::map<std::string, std::pair<int,int>>> ProgressObserver;
typedef Observer<bool> DoneObserver;

/**
 * @class ProgressReporter
 * This interface is implemented by the concrete progress reporters, so at run time
 * the software can switch the way of informing the user about the progress.
 * Details of what is reported is abstracted via key/value.
 */
class ProgressReporter: public ProgressObserver, public DoneObserver {
public:

  virtual ~ProgressReporter() = default;

  ProgressReporter() = default;

  /**
   * To be called when there are updates on the progress
   * @param info
   *    A dictionary, where the key is the task or subprocess name (i.e. segmentation),
   *    and the value a pair, where the first is the number of entities processed, and
   *    the second the total, or -1 if unknown.
   */
  void handleMessage(const std::map<std::string, std::pair<int, int>> & info) override {
    m_progress_info = info;
  }

  /**
   * To be called when the full processing is done.
   */
  void handleMessage(const bool& done) override {
    m_done = done;
  }

protected:
  std::map<std::string, std::pair<int, int>> m_progress_info;
  bool m_done = false;
};

} // end SExtractor

#endif // _SEMAIN_PROGRESSPRINTER_H
