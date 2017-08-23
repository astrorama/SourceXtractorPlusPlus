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

/// ExampleDeblendStep: Remove the source with lowest SimpleIntProperty value
class ExampleDeblendStep : public DeblendStep {
public:
  virtual void deblend(SourceGroupInterface& group) const {
    int min_value = INT_MAX;
    SourceGroupInterface::iterator min_value_iter = group.end();

    for(auto iter = group.begin(); iter != group.end(); ++iter) {

      auto value = iter->getProperty<SimpleIntProperty>().m_value;
      if (value < min_value) {
        min_value = value;
        min_value_iter = iter;
      }
    }

    if (min_value_iter != group.end()) {
      group.removeSource(min_value_iter);
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
  std::shared_ptr<ExampleDeblendStep> example_deblend_step {new ExampleDeblendStep};
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

  Deblending deblending({example_deblend_step});
  deblending.addObserver(test_group_observer);

  deblending.handleMessage(source_group);

  BOOST_CHECK_EQUAL(test_group_observer->m_groups.size(), 1);
  auto group = test_group_observer->m_groups.front();

  std::set<int> expected_result {2, 3};
  std::set<int> actual_result;

  for (auto& source : *group) {
    actual_result.insert(source.getProperty<SimpleIntProperty>().m_value);
  }

  BOOST_CHECK(expected_result == actual_result);
}

//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE( deblending_test_b, DeblendingFixture ) {
  source_a->setProperty<SimpleIntProperty>(1);
  source_b->setProperty<SimpleIntProperty>(2);
  source_c->setProperty<SimpleIntProperty>(3);

  // we want to execute step twice
  Deblending deblending({example_deblend_step, example_deblend_step});
  deblending.addObserver(test_group_observer);

  deblending.handleMessage(source_group);

  BOOST_CHECK(test_group_observer->m_groups.size() == 1);
  auto group = test_group_observer->m_groups.front();

  std::set<int> expected_result {3};
  std::set<int> actual_result;

  for (auto& source : *group) {
    actual_result.insert(source.getProperty<SimpleIntProperty>().m_value);
  }

  BOOST_CHECK(expected_result == actual_result);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


