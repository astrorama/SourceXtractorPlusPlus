/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/**
 * @file tests/src/SourceGroupWithOnDemandProperties_test.cpp
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
#include "SEFramework/Source/SourceWithOnDemandProperties.h"
#include "SEFramework/Property/Property.h"

#include "SEFramework/Source/SourceGroupWithOnDemandProperties.h"

using namespace SourceXtractor;

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

  virtual void computeProperties(SourceGroupInterface& group) const {
    group.setProperty<GroupProperty>(m_value);
  }

private:
  int m_value;
};

class GroupedSourceTask : public GroupTask {

public:

  GroupedSourceTask(int value) : m_value(value) {}

  virtual void computeProperties(SourceGroupInterface& group) const {
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
  SourceGroupWithOnDemandProperties group {mock_registry};

  const int magic_number = 42;

  SourceGroupFixture() {
    std::vector<std::unique_ptr<SourceInterface>> sources;
    sources.emplace_back(new SourceWithOnDemandProperties(mock_registry));
    sources.emplace_back(new SourceWithOnDemandProperties(mock_registry));
    group.addAllSources(std::move(sources));
  }

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


