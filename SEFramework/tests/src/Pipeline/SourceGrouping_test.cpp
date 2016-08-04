/**
 * @file tests/src/SourceGrouping_test.cpp
 * @date 05/24/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Property/Property.h"
#include "SEFramework/Task/TaskRegistry.h"

#include "SEFramework/Pipeline/SourceGrouping.h"

#include <memory>
#include <utility>

using namespace SExtractor;

// Example Property containing an int
class SimpleIntProperty : public Property {
public:
  int m_value;

  SimpleIntProperty(int value) : m_value(value) {}
};

class TestGroupingCriteria : public GroupingCriteria {
  virtual bool shouldGroup(const SourceInterface& first, const SourceInterface& second) const {

    auto first_group_id = first.getProperty<SimpleIntProperty>().m_value;
    auto second_group_id = second.getProperty<SimpleIntProperty>().m_value;

    return first_group_id == second_group_id;
  }
};

class SourceListObserver : public Observer<std::shared_ptr<SourceList>> {
public:
  virtual void handleMessage(const std::shared_ptr<SourceList>& source_list) override {
      m_list.push_back(source_list->getSources());
  }

  std::list<std::list<std::shared_ptr<Source>>> m_list;
};

struct SourceGroupingFixture {
  SelectAllCriteria select_all_criteria;

  std::shared_ptr<SourceGrouping> source_grouping;
  std::shared_ptr<TaskRegistry> task_registry;
  std::shared_ptr<Source> source_a, source_b, source_c;
  std::shared_ptr<SourceListObserver> source_list_observer;

  SourceGroupingFixture()
    : source_grouping(new SourceGrouping(
        std::unique_ptr<GroupingCriteria>(new TestGroupingCriteria), SourceList::getFactory())),
      task_registry(new TaskRegistry()),
      source_a(new Source(task_registry)),
      source_b(new Source(task_registry)),
      source_c(new Source(task_registry)),
      source_list_observer(new SourceListObserver) {
    source_grouping->addObserver(source_list_observer);
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SourceGrouping_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( source_grouping_test, SourceGroupingFixture ) {
  source_a->setProperty<SimpleIntProperty>(1);
  source_b->setProperty<SimpleIntProperty>(2);
  source_c->setProperty<SimpleIntProperty>(1);

  source_grouping->handleMessage(source_a);
  source_grouping->handleMessage(source_b);
  source_grouping->handleMessage(source_c);

  source_grouping->handleMessage(ProcessSourcesEvent { select_all_criteria } );

  BOOST_CHECK(source_list_observer->m_list.size() == 2);
  BOOST_CHECK(source_list_observer->m_list == std::list<std::list<std::shared_ptr<Source>>> ({
    std::list<std::shared_ptr<Source>>( {source_a, source_c} ),
    std::list<std::shared_ptr<Source>>( {source_b} )
  }));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( process_sources_test, SourceGroupingFixture ) {
  source_a->setProperty<SimpleIntProperty>(1);
  source_b->setProperty<SimpleIntProperty>(2);
  source_c->setProperty<SimpleIntProperty>(1);

  source_grouping->handleMessage(source_a);
  source_grouping->handleMessage(source_b);

  // Process all sources currently in source_grouping
  source_grouping->handleMessage(ProcessSourcesEvent { select_all_criteria } );

  source_grouping->handleMessage(source_c);

  // Process all sources
  source_grouping->handleMessage(ProcessSourcesEvent { select_all_criteria } );

  // this time the sources were not grouped due to them being processed before they could be grouped
  BOOST_CHECK(source_list_observer->m_list.size() == 3);
  BOOST_CHECK(source_list_observer->m_list == std::list<std::list<std::shared_ptr<Source>>> ({
    std::list<std::shared_ptr<Source>>( {source_a} ),
    std::list<std::shared_ptr<Source>>( {source_b} ),
    std::list<std::shared_ptr<Source>>( {source_c} )
  }));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


