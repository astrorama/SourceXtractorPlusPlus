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
 * @file tests/src/PixelRefinement_test.cpp
 * @date 05/23/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/EnableGMock.h"
using namespace testing;

#include "SEFramework/Pipeline/Partition.h"
#include "SEFramework/Source/SimpleSource.h"
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
  virtual std::vector<std::shared_ptr<SourceInterface>> partition(std::shared_ptr<SourceInterface> source) const {
    return { source };
  }
};

class ExamplePartitionStep : public PartitionStep {
public:
  virtual std::vector<std::shared_ptr<SourceInterface>> partition(std::shared_ptr<SourceInterface> source) const {
    auto& property = source->getProperty<SimpleIntProperty>();
    if (property.m_value % 2 != 0) {
      return { source };
    } else {
      int newValue = property.m_value / 2;
      source->setProperty<SimpleIntProperty>(newValue);

      auto new_source = std::make_shared<SimpleSource>();
      new_source->setProperty<SimpleIntProperty>(newValue);

      return { source, new_source };
    }
  }
};

class MockSourceObserver : public Observer<std::shared_ptr<SourceInterface>> {
public:
  MOCK_METHOD1(handleMessage, void (const std::shared_ptr<SourceInterface>&));
};

struct RefineSourceFixture {
  std::shared_ptr<NopPartitionStep> nop_step {new NopPartitionStep};
  std::shared_ptr<ExamplePartitionStep> example_step {new ExamplePartitionStep};
  std::shared_ptr<MockSourceObserver> mock_observer {new MockSourceObserver};
  std::shared_ptr<SourceInterface> source {new SimpleSource};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PixelRefinement_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( default_behavior_test, RefineSourceFixture ) {
  // We want to test a Partition with no PartitionStep at all
  Partition partition( {} );

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

  // We expect to get our Source back unchanged
  EXPECT_CALL(*mock_observer, handleMessage(source)).Times(1);

  // Add the Observer
  partition.addObserver(mock_observer);

  // And process the Source
  partition.handleMessage(source);
}

BOOST_FIXTURE_TEST_CASE( example_step_test, RefineSourceFixture ) {
  Partition partition( { example_step, nop_step, example_step, example_step } );

  source->setProperty<SimpleIntProperty>(4);

  EXPECT_CALL(*mock_observer, handleMessage(_)).Times(4);

  partition.addObserver(mock_observer);
  partition.handleMessage(source);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

