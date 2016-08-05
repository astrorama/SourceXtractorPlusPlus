/**
 * @file tests/src/SourceGrouping_test.cpp
 * @date 05/24/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Property/Property.h"
#include "SEFramework/Task/TaskProvider.h"

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

struct IdProperty : public Property {
  std::string id;
  IdProperty(std::string id) : id(id) { }
};

class TestGroupingCriteria : public GroupingCriteria {
  virtual bool shouldGroup(const SourceInterface& first, const SourceInterface& second) const {

    auto first_group_id = first.getProperty<SimpleIntProperty>().m_value;
    auto second_group_id = second.getProperty<SimpleIntProperty>().m_value;

    return first_group_id == second_group_id;
  }
};

class SourceGroupObserver : public Observer<std::shared_ptr<SourceGroup>> {
public:
  virtual void handleMessage(const std::shared_ptr<SourceGroup>& group) override {
      m_list.push_back(group);
  }

  std::vector<std::shared_ptr<SourceGroup>> m_list;
};

struct SourceGroupingFixture {
  std::shared_ptr<TaskProvider> task_provider;
  SelectAllCriteria select_all_criteria;

  std::shared_ptr<SourceGrouping> source_grouping;
  std::shared_ptr<Source> source_a, source_b, source_c;
  std::shared_ptr<SourceGroupObserver> source_group_observer;

  SourceGroupingFixture()
    : task_provider(new TaskProvider(nullptr)),
      source_grouping(new SourceGrouping(
        std::unique_ptr<GroupingCriteria>(new TestGroupingCriteria), SourceList::getFactory(), task_provider)),
      source_a(new Source(task_provider)),
      source_b(new Source(task_provider)),
      source_c(new Source(task_provider)),
      source_group_observer(new SourceGroupObserver) {
    source_grouping->addObserver(source_group_observer);
    source_a->setProperty<IdProperty>("A");
    source_b->setProperty<IdProperty>("B");
    source_c->setProperty<IdProperty>("C");
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

  BOOST_CHECK(source_group_observer->m_list.size() == 2);
  auto group = source_group_observer->m_list[0];
  auto iter = group->begin();
  BOOST_CHECK_EQUAL(iter->getProperty<IdProperty>().id, "A");
  ++iter;
  BOOST_CHECK_EQUAL(iter->getProperty<IdProperty>().id, "C");
  ++iter;
  BOOST_CHECK(iter == group->end());
  group = source_group_observer->m_list[1];
  iter = group->begin();
  BOOST_CHECK_EQUAL(iter->getProperty<IdProperty>().id, "B");
  ++iter;
  BOOST_CHECK(iter == group->end());
  
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
  BOOST_CHECK(source_group_observer->m_list.size() == 3);
  auto group = source_group_observer->m_list[0];
  auto iter = group->begin();
  BOOST_CHECK_EQUAL(iter->getProperty<IdProperty>().id, "A");
  ++iter;
  BOOST_CHECK(iter == group->end());
  group = source_group_observer->m_list[1];
  iter = group->begin();
  BOOST_CHECK_EQUAL(iter->getProperty<IdProperty>().id, "B");
  ++iter;
  BOOST_CHECK(iter == group->end());
  group = source_group_observer->m_list[2];
  iter = group->begin();
  BOOST_CHECK_EQUAL(iter->getProperty<IdProperty>().id, "C");
  ++iter;
  BOOST_CHECK(iter == group->end());
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


