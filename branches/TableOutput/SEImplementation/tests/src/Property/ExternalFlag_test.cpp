/**
 * @file tests/src/ExternalFlag_test.cpp
 * @date 06/13/16
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/Source.h"
#include "SEFramework/Task/TaskRegistry.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Property/ExternalFlag.h"
#include "SEImplementation/Task/ExternalFlagTask.h"

using namespace SExtractor;

struct ExternalFlagFixture {
  std::shared_ptr<TaskRegistry> task_registry;
  std::shared_ptr<Source> source;

  ExternalFlagFixture() : task_registry(new TaskRegistry) {
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ExternalFlag_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( external_flag_or_test, ExternalFlagFixture ) {
  source.reset(new Source(
      std::vector<PixelCoordinate>( { PixelCoordinate(1, 0), PixelCoordinate(2, 0) } ), task_registry));

  auto flag_image = std::make_shared<VectorImage<std::int64_t>>(4, 1, std::vector<std::int64_t> {0, 1, 2, 3} );

  ExternalFlagTaskOr task(flag_image, 0);
  task.computeProperties(*source);

  auto& external_flag_property = source->getProperty<ExternalFlag>();

  BOOST_CHECK_EQUAL(external_flag_property.getFlag(), 3);
  BOOST_CHECK_EQUAL(external_flag_property.getCount(), 2);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( external_flag_and_test, ExternalFlagFixture ) {
  source.reset(new Source(
      std::vector<PixelCoordinate>( { PixelCoordinate(1, 0), PixelCoordinate(3, 0) } ), task_registry));

  auto flag_image = std::make_shared<VectorImage<std::int64_t>>(4, 1, std::vector<std::int64_t> {0, 1, 2, 3} );

  ExternalFlagTaskAnd task(flag_image, 0);
  task.computeProperties(*source);

  auto& external_flag_property = source->getProperty<ExternalFlag>();

  BOOST_CHECK_EQUAL(external_flag_property.getFlag(), 1);
  BOOST_CHECK_EQUAL(external_flag_property.getCount(), 2);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( external_flag_min_test, ExternalFlagFixture ) {
  source.reset(new Source(
      std::vector<PixelCoordinate>( { PixelCoordinate(1, 0), PixelCoordinate(3, 0) } ), task_registry));

  auto flag_image = std::make_shared<VectorImage<std::int64_t>>(4, 1, std::vector<std::int64_t> {0, 1, 2, 3} );

  ExternalFlagTaskMin task(flag_image, 0);
  task.computeProperties(*source);

  auto& external_flag_property = source->getProperty<ExternalFlag>();

  BOOST_CHECK_EQUAL(external_flag_property.getFlag(), 1);
  BOOST_CHECK_EQUAL(external_flag_property.getCount(), 1);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( external_flag_max_test, ExternalFlagFixture ) {
  source.reset(new Source(
      std::vector<PixelCoordinate>( { PixelCoordinate(1, 0), PixelCoordinate(3, 0) } ), task_registry));

  auto flag_image = std::make_shared<VectorImage<std::int64_t>>(4, 1, std::vector<std::int64_t> {0, 1, 2, 3} );

  ExternalFlagTaskMax task(flag_image, 0);
  task.computeProperties(*source);

  auto& external_flag_property = source->getProperty<ExternalFlag>();

  BOOST_CHECK_EQUAL(external_flag_property.getFlag(), 3);
  BOOST_CHECK_EQUAL(external_flag_property.getCount(), 1);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( external_flag_most_test, ExternalFlagFixture ) {
  source.reset(new Source(
      std::vector<PixelCoordinate>( { PixelCoordinate(1, 0), PixelCoordinate(2, 0), PixelCoordinate(3, 0) } ), task_registry));

  auto flag_image = std::make_shared<VectorImage<std::int64_t>>(4, 1, std::vector<std::int64_t> {0, 1, 1, 3} );

  ExternalFlagTaskMost task(flag_image, 0);
  task.computeProperties(*source);

  auto& external_flag_property = source->getProperty<ExternalFlag>();

  BOOST_CHECK_EQUAL(external_flag_property.getFlag(), 1);
  BOOST_CHECK_EQUAL(external_flag_property.getCount(), 2);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
