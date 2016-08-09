/**
 * @file tests/src/Deblending_test.cpp
 * @date 05/26/16
 * @author mschefer
 */

#include <iterator>
#include <boost/test/unit_test.hpp>

#include "SEFramework/Property/Property.h"
#include "SEFramework/Pipeline/Deblending.h"
#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Source/SimpleSourceGroup.h"

using namespace SExtractor;

// Example Property containing an int
class SimpleIntProperty : public Property {
public:
  int m_value;

  SimpleIntProperty(int value) : m_value(value) {}
};

/// ExampleDeblendAction: if SourceList has at least 2 elements, remove the first element
class ExampleDeblendAction : public DeblendAction {
public:
  virtual void deblend(SourceGroupInterface& group) const {
    if (std::distance(group.begin(), group.end()) >= 2) {
      group.removeSource(group.begin());
    }
  }
};

class TestGroupObserver : public Observer<std::shared_ptr<SourceGroupInterface>> {
public:
  virtual void handleMessage(const std::shared_ptr<SourceGroupInterface>& source_group) override {
    m_groups.push_back(source_group);
  }

  std::list<std::shared_ptr<SourceGroupInterface>> m_groups;
};

struct DeblendingFixture {
  std::shared_ptr<ExampleDeblendAction> example_deblend_action {new ExampleDeblendAction};
  std::shared_ptr<SourceInterface> source_a {new SimpleSource};
  std::shared_ptr<SourceInterface> source_b {new SimpleSource};
  std::shared_ptr<SourceInterface> source_c {new SimpleSource};
  std::shared_ptr<SourceGroupInterface> source_group {new SimpleSourceGroup};
  std::shared_ptr<TestGroupObserver> test_group_observer {new TestGroupObserver};

  DeblendingFixture() {
    source_group->addSource(source_a);
    source_group->addSource(source_b);
    source_group->addSource(source_c);
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (Deblending_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( deblending_test_a, DeblendingFixture ) {
  source_a->setProperty<SimpleIntProperty>(1);
  source_b->setProperty<SimpleIntProperty>(2);
  source_c->setProperty<SimpleIntProperty>(3);

  Deblending deblending({example_deblend_action});
  deblending.addObserver(test_group_observer);

  deblending.handleMessage(source_group);

  BOOST_CHECK(test_group_observer->m_groups.size() == 1);
  auto group = test_group_observer->m_groups.front();
  auto iter = group->begin();
  BOOST_CHECK(iter != group->end());
  BOOST_CHECK(iter->getProperty<SimpleIntProperty>().m_value == 2);
  ++iter;
  BOOST_CHECK(iter != group->end());
  BOOST_CHECK(iter->getProperty<SimpleIntProperty>().m_value == 3);
  ++iter;
  BOOST_CHECK(iter == group->end());
}

//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE( deblending_test_b, DeblendingFixture ) {
  source_a->setProperty<SimpleIntProperty>(1);
  source_b->setProperty<SimpleIntProperty>(2);
  source_c->setProperty<SimpleIntProperty>(3);

  // we want to execute example_deblend_action twice
  Deblending deblending({example_deblend_action, example_deblend_action});
  deblending.addObserver(test_group_observer);

  deblending.handleMessage(source_group);

  BOOST_CHECK(test_group_observer->m_groups.size() == 1);
  auto group = test_group_observer->m_groups.front();
  auto iter = group->begin();
  BOOST_CHECK(iter != group->end());
  BOOST_CHECK(iter->getProperty<SimpleIntProperty>().m_value == 3);
  ++iter;
  BOOST_CHECK(iter == group->end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


