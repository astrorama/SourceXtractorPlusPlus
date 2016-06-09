/**
 * @file src/lib/Source/SourceList.cpp
 * @date 05/24/16
 * @author mschefer
 */
#include "SEFramework/Source/SourceList.h"

namespace SEFramework {

SourceList::SourceList(std::vector<std::shared_ptr<Source>> sources) {
  for (auto& source : sources) {
    addSource(source);
  }
}

void SourceList::addSource(std::shared_ptr<Source> source) {
  m_sources.push_back(source);
}

void SourceList::removeSource(std::shared_ptr<Source> source) {
  m_sources.remove(source);
}

void SourceList::merge(SourceList& source_list) {
  auto& other_sources = source_list.getSources();
  m_sources.insert(m_sources.end(), other_sources.begin(), other_sources.end());
}

const std::list<std::shared_ptr<Source>>& SourceList::getSources() const {
  return m_sources;
}

} // SEFramework namespace



