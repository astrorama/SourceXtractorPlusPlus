/**
 * @file tests/src/ExternalFlag_test.cpp
 * @date 06/13/16
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Plugin/ExternalFlag/ExternalFlag.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/ExternalFlag/ExternalFlagTask.h"

using namespace SExtractor;

struct ExternalFlagFixture {
  SimpleSource source;

  ExternalFlagFixture() {
    source.setProperty<DetectionFrame>(
      std::make_shared<DetectionImageFrame>(ConstantImage<SeFloat>::create(4, 1, 0),
                                            nullptr, 1e6, nullptr, 1, 1, 0));
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ExternalFlag_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( external_flag_or_test, ExternalFlagFixture ) {
  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1, 0}, {2, 0}});

  auto flag_image = VectorImage<std::int64_t>::create(4, 1, std::vector<std::int64_t> {0, 1, 2, 3} );

  ExternalFlagTaskOr task(flag_image, 0);
  task.computeProperties(source);

  auto& external_flag_property = source.getProperty<ExternalFlag>();

  BOOST_CHECK_EQUAL(external_flag_property.getFlag(), 3);
  BOOST_CHECK_EQUAL(external_flag_property.getCount(), 2);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( external_flag_and_test, ExternalFlagFixture ) {
  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1, 0}, {3, 0}});

  auto flag_image = VectorImage<std::int64_t>::create(4, 1, std::vector<std::int64_t> {0, 1, 2, 3} );

  ExternalFlagTaskAnd task(flag_image, 0);
  task.computeProperties(source);

  auto& external_flag_property = source.getProperty<ExternalFlag>();

  BOOST_CHECK_EQUAL(external_flag_property.getFlag(), 1);
  BOOST_CHECK_EQUAL(external_flag_property.getCount(), 2);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( external_flag_min_test, ExternalFlagFixture ) {
  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1, 0}, {3, 0}});

  auto flag_image = VectorImage<std::int64_t>::create(4, 1, std::vector<std::int64_t> {0, 1, 2, 3} );

  ExternalFlagTaskMin task(flag_image, 0);
  task.computeProperties(source);

  auto& external_flag_property = source.getProperty<ExternalFlag>();

  BOOST_CHECK_EQUAL(external_flag_property.getFlag(), 1);
  BOOST_CHECK_EQUAL(external_flag_property.getCount(), 1);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( external_flag_max_test, ExternalFlagFixture ) {
  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1, 0}, {3, 0}});

  auto flag_image = VectorImage<std::int64_t>::create(4, 1, std::vector<std::int64_t> {0, 1, 2, 3} );

  ExternalFlagTaskMax task(flag_image, 0);
  task.computeProperties(source);

  auto& external_flag_property = source.getProperty<ExternalFlag>();

  BOOST_CHECK_EQUAL(external_flag_property.getFlag(), 3);
  BOOST_CHECK_EQUAL(external_flag_property.getCount(), 1);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( external_flag_most_test, ExternalFlagFixture ) {
  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1, 0}, {2, 0}, {3, 0}});

  auto flag_image = VectorImage<std::int64_t>::create(4, 1, std::vector<std::int64_t> {0, 1, 1, 3} );

  ExternalFlagTaskMost task(flag_image, 0);
  task.computeProperties(source);

  auto& external_flag_property = source.getProperty<ExternalFlag>();

  BOOST_CHECK_EQUAL(external_flag_property.getFlag(), 1);
  BOOST_CHECK_EQUAL(external_flag_property.getCount(), 2);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
