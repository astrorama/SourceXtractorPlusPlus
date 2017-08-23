/*
 * History.h
 *
 *  Created on: Jun 28, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_HISTORY_HISTORY_H_
#define _SEFRAMEWORK_HISTORY_HISTORY_H_

namespace SExtractor {

class HistoryEntry {
public:
  virtual ~HistoryEntry() = default;
};

class History {
public:

  virtual ~History() = default;

  virtual void addHistoryEntry(std::unique_ptr<HistoryEntry> entry) {
    m_entries.emplace_back(std::move(entry));
  }

  virtual const std::vector<std::unique_ptr<const HistoryEntry>>& getHistory() {
    return m_entries;
  }

private:
  std::vector<std::unique_ptr<const HistoryEntry>> m_entries;
};

}



#endif /* _SEFRAMEWORK_HISTORY_HISTORY_H_ */
