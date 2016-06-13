/**
 * @file SEFramework/Source/SourceList.h
 * @date 05/24/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_SOURCE_SOURCELIST_H
#define _SEFRAMEWORK_SOURCE_SOURCELIST_H

#include <list>
#include <functional>

#include "SEFramework/Source/Source.h"

namespace SExtractor {

/**
 * @class SourceList
 * @brief
 *
 */
class SourceList {

public:

  /**
   * @brief Destructor
   */
  virtual ~SourceList() = default;

  virtual void addSource(std::shared_ptr<Source> source);
  virtual void removeSource(std::shared_ptr<Source> source);
  virtual void merge(SourceList& source_list);

  virtual const std::list<std::shared_ptr<Source>>& getSources() const;

  using SourceListFactory = std::function<std::unique_ptr<SourceList>(const std::vector<std::shared_ptr<Source>>&)>;

  template <class T = SourceList>
  static SourceListFactory getFactory() {
    return [](const std::vector<std::shared_ptr<Source>>& sources) {
      return std::unique_ptr<SourceList>(new T(std::move(sources)));
    };
  }

protected:
  explicit SourceList(const std::vector<std::shared_ptr<Source>>& sources);

  std::list<std::shared_ptr<Source>> m_sources;

private:

}; /* End of SourceList class */


} /* namespace SEFramework */


#endif
