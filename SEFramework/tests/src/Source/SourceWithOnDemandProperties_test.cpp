/**
 * @file tests/src/Source_test.cpp
 * @date 05/06/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/EnableGMock.h"
using namespace testing;

#include "SEFramework/Source/SourceWithOnDemandProperties.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Task/TaskProvider.h"
#include "SEFramework/Property/Property.h"

using namespace SExtractor;


// Example Property containing an int
class SimpleIntProperty : public Property {
public:
  int m_value;

  SimpleIntProperty(int value) : m_value(value) {}
};

// A task to create SimpleIntProperty
class SimpleIntPropertyTask : public SourceTask {
public:
  SimpleIntPropertyTask(int value) : m_value(value) {}

  virtual void computeProperties(SourceInterface& source) const override {
    source.setProperty<SimpleIntProperty>(m_value);
  }

private:
  int m_value;
};

// Mock for the TaskProvider so that we can check interactions with it
class MockTaskProvider : public TaskProvider {
public:
  MockTaskProvider() : TaskProvider(nullptr) {}

  MOCK_CONST_METHOD1(getTask, std::shared_ptr<const Task> (const PropertyId& property_id));
};

struct SourceFixture {

  std::shared_ptr<MockTaskProvider> mock_registry;
  SourceWithOnDemandProperties source;

  const int magic_number = 42;

  SourceFixture() :
    mock_registry(std::make_shared<MockTaskProvider>()),
    source(mock_registry) {}
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SourceWithOnDemandProperties_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( SourceWithOnDemandProperties_getProperty_test, SourceFixture ) {

  // getTask() should only be called once
  EXPECT_CALL(*mock_registry, getTask(_))
      .Times(1)
      .WillOnce(Return(std::make_shared<SimpleIntPropertyTask>(magic_number)));

  // First getProperty() call triggers getTask()
  auto& property = source.getProperty<SimpleIntProperty>();

  // check that our property contains the correct value
  BOOST_CHECK_EQUAL(property.m_value, magic_number);

  // The second getProperty() call should not trigger a second getTask()
  auto& same_property = source.getProperty<SimpleIntProperty>();

  // check that our property contains the correct value
  BOOST_CHECK_EQUAL(same_property.m_value, magic_number);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

