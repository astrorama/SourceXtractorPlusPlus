/**
 * @file SEFramework/Pipeline/SourceGrouping.h
 * @date 05/24/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_PIPELINE_SOURCEGROUPING_H
#define _SEFRAMEWORK_PIPELINE_SOURCEGROUPING_H

#include <memory>
#include <list>

#include "SEFramework/Source/Source.h"
#include "SEFramework/Source/SourceList.h"

#include "SEUtils/Observable.h"

namespace SEFramework {

class SelectionCriteria {
public:
  /**
   * @brief Destructor
   */
  virtual ~SelectionCriteria() = default;

  virtual bool mustBeProcessed(const Source& source) const = 0;
};

struct ProcessSourcesEvent {
  const SelectionCriteria& m_selection_criteria;

  ProcessSourcesEvent(const SelectionCriteria& selection_criteria) : m_selection_criteria(selection_criteria) {}
};

class GroupingCriteria {
public:
  /**
   * @brief Destructor
   */
  virtual ~GroupingCriteria() = default;

  virtual bool shouldGroup(const SourceList& source_list, const Source& source) const = 0;
};

/**
 * @class SourceGrouping
 * @brief
 *
 */
class SourceGrouping : public SEUtils::Observer<std::shared_ptr<Source>>,
    public SEUtils::Observer<ProcessSourcesEvent>, public SEUtils::Observable<std::shared_ptr<SourceList>> {
public:

  /**
   * @brief Destructor
   */
  virtual ~SourceGrouping() = default;

  SourceGrouping(std::unique_ptr<GroupingCriteria> grouping_criteria,
      SourceList::SourceListFactory source_list_factory);

  virtual void handleMessage(const std::shared_ptr<Source>& source) override;
  virtual void handleMessage(const ProcessSourcesEvent& source) override;

private:
  std::unique_ptr<GroupingCriteria> m_grouping_criteria;
  std::list<std::shared_ptr<SourceList>> m_source_lists;

  SourceList::SourceListFactory m_source_list_factory;

}; /* End of SourceGrouping class */


class SelectAllCriteria : public SelectionCriteria {
public:
  virtual bool mustBeProcessed(const Source& ) const override {
    return true;
  }
};

} /* namespace SEFramework */


#endif
