/*
 * SourceHistory.h
 *
 *  Created on: Jul 7, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_HISTORY_SOURCEHISTORY_H_
#define _SEFRAMEWORK_HISTORY_SOURCEHISTORY_H_

#include "SEFramework/History/History.h"

namespace SExtractor {

class Source;

class SourceHistory : public HistoryEntry {
public:

  SourceHistory(std::shared_ptr<Source> source) : m_source(source) {}

  std::shared_ptr<Source> getSource() const { return m_source; }

private:
  std::shared_ptr<Source> m_source;
};

}

#endif /* _SEFRAMEWORK_HISTORY_SOURCEHISTORY_H_ */
