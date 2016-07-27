/**
 * @file tests/src/PixelRefinement_test.cpp
 * @date 05/23/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/EnableGMock.h"
using namespace testing;

#include "SEFramework/Pipeline/Partition.h"
#include "SEFramework/Source/Source.h"
#include "SEFramework/Task/TaskRegistry.h"
#include "SEFramework/Property/Property.h"

using namespace SExtractor;

// Example property containing an int
class SimpleIntProperty : public Property {
public:
  int m_value;

  SimpleIntProperty(int value) : m_value(value) {}
};

class NopPartitionStep : public PartitionStep {
public:
  virtual std::vector<std::shared_ptr<Source>> partition(std::shared_ptr<Source> source) const {
    return { source };
  }
};

class ExamplePartitionStep : public PartitionStep {
public:
  virtual std::vector<std::shared_ptr<Source>> partition(std::shared_ptr<Source> source) const {
    auto& property = source->getProperty<SimpleIntProperty>();
    if (property.m_value % 2 != 0) {
      return { source };
    } else {
      int newValue = property.m_value / 2;
      source->setProperty(std::unique_ptr<SimpleIntProperty>(new SimpleIntProperty(newValue)));

      auto new_source = std::make_shared<Source>(source->getPixels(), m_task_registry);
      new_source->setProperty(std::unique_ptr<SimpleIntProperty>(new SimpleIntProperty(newValue)));

      return { source, new_source };
    }
  }

private:
  std::shared_ptr<TaskRegistry> m_task_registry;
};

class MockSourceObserver : public Observer<std::shared_ptr<Source>> {
public:
  MOCK_METHOD1(handleMessage, void (const std::shared_ptr<Source>&));
};

struct RefineSourceFixture {
  std::shared_ptr<NopPartitionStep> nop_step;
  std::shared_ptr<ExamplePartitionStep> example_step;
  std::shared_ptr<TaskRegistry> task_registry;
  std::shared_ptr<MockSourceObserver> mock_observer;

  RefineSourceFixture()
    : nop_step(new NopPartitionStep),
      example_step(new ExamplePartitionStep),
      task_registry(new TaskRegistry),
      mock_observer(new MockSourceObserver) {
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PixelRefinement_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( default_behavior_test, RefineSourceFixture ) {
  // We want to test a Partition with no PartitionStep at all
  Partition partition( {} );

  // Make a source
  auto source = std::make_shared<Source>(std::vector<PixelCoordinate>(), task_registry);

  // We expect to get our Source back unchanged
  EXPECT_CALL(*mock_observer, handleMessage(source)).Times(1);

  // Add the Observer
  partition.addObserver(mock_observer);

  // And process the Source
  partition.handleMessage(source);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( nop_step_test, RefineSourceFixture ) {
  // A Partition with a single PartitionStep that does nothing
  Partition partition( { nop_step } );

  // Make a source
  auto source = std::make_shared<Source>(std::vector<PixelCoordinate>(), task_registry);

  // We expect to get our Source back unchanged
  EXPECT_CALL(*mock_observer, handleMessage(source)).Times(1);

  // Add the Observer
  partition.addObserver(mock_observer);

  // And process the Source
  partition.handleMessage(source);
}

BOOST_FIXTURE_TEST_CASE( example_step_test, RefineSourceFixture ) {
  Partition partition( { example_step, nop_step, example_step, example_step } );
  auto source = std::make_shared<Source>(std::vector<PixelCoordinate>(), task_registry);
  source->setProperty(std::unique_ptr<SimpleIntProperty>(new SimpleIntProperty(4)));

  EXPECT_CALL(*mock_observer, handleMessage(_)).Times(4);

  partition.addObserver(mock_observer);
  partition.handleMessage(source);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

