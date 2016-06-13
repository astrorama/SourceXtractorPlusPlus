/**
 * @file tests/src/EntangledSourceGroup_test.cpp
 * @date 05/09/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/EnableGMock.h"
using namespace testing;

#include "SEUtils/PixelCoordinate.h"

#include "SEFramework/Task/TaskRegistry.h"
#include "SEFramework/Task/GroupTask.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Source/Source.h"

#include "SEFramework/Source/EntangledSourceGroup.h"

using namespace SExtractor;

// Mock for the TaskRegistry so that we can check interactions with it
class MockTaskRegistry : public TaskRegistry {
public:
  MOCK_CONST_METHOD1(getTask, std::shared_ptr<Task> (PropertyId property_id));
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

  virtual void computeProperties(EntangledSourceGroup& group) {
    group.setProperty(std::unique_ptr<GroupProperty>(new GroupProperty(m_value)));
  }

private:
  int m_value;
};

class GroupedSourceTask : public GroupTask {

public:

  GroupedSourceTask(int value) : m_value(value) {}

  virtual void computeProperties(EntangledSourceGroup& group) {
    // Sets the property on all the sources in the group
    for (auto source : group.getSources()) {
      source->setProperty(std::unique_ptr<SourceProperty>(new SourceProperty(m_value)));
    }
  }

private:
  int m_value;
};

struct EntangledSourceGroupFixture {

  std::shared_ptr<MockTaskRegistry> mock_registry;
  std::shared_ptr<Source> source_a, source_b;
  std::shared_ptr<EntangledSourceGroup> group;

  const int magic_number = 42;

  EntangledSourceGroupFixture() :
    mock_registry(std::make_shared<MockTaskRegistry>()),
    source_a(new Source(std::vector<PixelCoordinate>(), mock_registry)),
    source_b(new Source(std::vector<PixelCoordinate>(), mock_registry)),
    group(EntangledSourceGroup::create({source_a, source_b}, mock_registry)) {
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (EntangledSourceGroup_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( example_test, EntangledSourceGroupFixture ) {

  // We expect to only request the Task to compute the property once
  EXPECT_CALL(*mock_registry, getTask(_))
      .Times(1)
      .WillOnce(Return(std::make_shared<GroupPropertyTask>(magic_number)));

  // First getProperty() call triggers getTask()

  auto& property = group->getProperty<GroupProperty>();
  BOOST_CHECK_EQUAL(property.m_value, magic_number);

  // The second getProperty() call should not trigger a second getTask()
  // instead we use the property we already have

  auto& same_property = group->getProperty<GroupProperty>();
  BOOST_CHECK_EQUAL(same_property.m_value, magic_number);
}

BOOST_FIXTURE_TEST_CASE( entangled_source_property_test, EntangledSourceGroupFixture ) {
  // We expect that first we will try to get a Source task then a Group task when that fails
  EXPECT_CALL(*mock_registry, getTask(PropertyId(typeid(SourceProperty))))
      .Times(2)
      .WillRepeatedly(Return(std::make_shared<GroupedSourceTask>(magic_number)));

  auto sources = group->getSources();
  group.reset(); // the group should stay in memory anyway (we check that it does)

  // The first getProperty() call triggers 2 getTask() calls
  auto property = sources[0]->getProperty<SourceProperty>();
  BOOST_CHECK_EQUAL(property.m_value, magic_number);

  // The second getProperty() call should not trigger yet another getTask()
  property = sources[1]->getProperty<SourceProperty>();
  BOOST_CHECK_EQUAL(property.m_value, magic_number);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

