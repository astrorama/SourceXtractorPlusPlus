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
 * @brief Used to store related Sources together before they are grouped in an EntangledSourceGroup.
 *
 * @details Can be specialized to implement versions that keep track of some parameters as sources are added/removed.
 *
 */
class SourceList {

public:

  /// Destructor
  virtual ~SourceList() = default;

  /// Removes the copy/move constructors and assignment operators
  SourceList(const SourceList&) = delete;
  SourceList(SourceList&&) = delete;
  SourceList& operator=(const SourceList&) = delete;
  SourceList& operator=(SourceList&&) = delete;

  /// Adds a source to the list
  virtual void addSource(std::shared_ptr<Source> source);

  // Removes a source from the list
  virtual void removeSource(std::shared_ptr<Source> source);

  // Merges another list into the current list
  virtual void merge(const SourceList& source_list);

  // Gets a reference to the current list of Sources
  virtual const std::list<std::shared_ptr<Source>>& getSources() const;

  // defines a the signature for a factory function returning a unique_ptr to SourceList
  using SourceListFactory = std::function<std::unique_ptr<SourceList>(const std::vector<std::shared_ptr<Source>>&)>;

  // Static method to get a factory function that creates a SourceList. Templated to be usable with subclasses.
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


} /* namespace SExtractor */


#endif
