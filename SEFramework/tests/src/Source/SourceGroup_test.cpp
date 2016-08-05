/**
 * @file tests/src/SourceGroup_test.cpp
 * @date 08/04/16
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/EnableGMock.h"
using namespace testing;

#include "SEUtils/PixelCoordinate.h"

#include "SEFramework/Task/TaskProvider.h"
#include "SEFramework/Task/GroupTask.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Source/Source.h"
#include "SEFramework/Property/Property.h"

#include "SEFramework/Source/SourceGroup.h"

using namespace SExtractor;

// Mock for the TaskProvider so that we can check interactions with it
class MockTaskProvider : public TaskProvider {
public:
  MockTaskProvider() : TaskProvider(nullptr) {}

  MOCK_CONST_METHOD1(getTask, std::shared_ptr<const Task> (const PropertyId& property_id));
};

// Example properties
class SourceProperty : public Property {
public:
  int m_value;

  SourceProperty(int value) : m_value(value) {}
};

class GroupProperty : public Property {
public:
  int m_value;

  GroupProperty(int value) : m_value(value) {}
};

// Tasks to create the example properties and give them a value

class GroupPropertyTask : public GroupTask {

public:

  GroupPropertyTask(int value) : m_value(value) {}

  virtual void computeProperties(SourceGroup& group) const {
    group.setProperty<GroupProperty>(m_value);
  }

private:
  int m_value;
};

class GroupedSourceTask : public GroupTask {

public:

  GroupedSourceTask(int value) : m_value(value) {}

  virtual void computeProperties(SourceGroup& group) const {
    // Sets the property on all the sources in the group
    for (auto& source : group) {
      source.setProperty<SourceProperty>(m_value);
    }
  }

private:
  int m_value;
};

struct SourceGroupFixture {

  std::shared_ptr<MockTaskProvider> mock_registry {std::make_shared<MockTaskProvider>()};
  std::shared_ptr<Source> source_a {new Source(mock_registry)};
  std::shared_ptr<Source> source_b {new Source(mock_registry)};
  SourceGroup group {mock_registry, std::vector<std::shared_ptr<Source>>{source_a, source_b}};

  const int magic_number = 42;

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SourceGroup_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( example_test, SourceGroupFixture ) {

  // We expect to only request the Task to compute the property once
  EXPECT_CALL(*mock_registry, getTask(_))
      .Times(1)
      .WillOnce(Return(std::make_shared<GroupPropertyTask>(magic_number)));

  // First getProperty() call triggers getTask()

  auto& property = group.getProperty<GroupProperty>();
  BOOST_CHECK_EQUAL(property.m_value, magic_number);

  // The second getProperty() call should not trigger a second getTask()
  // instead we use the property we already have

  auto& same_property = group.getProperty<GroupProperty>();
  BOOST_CHECK_EQUAL(same_property.m_value, magic_number);
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


