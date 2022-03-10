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
 * @file tests/src/ExternalFlag_test.cpp
 * @date 06/13/16
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/DetectionFrameInfo/DetectionFrameInfo.h"

#include "SEImplementation/Plugin/ExternalFlag/ExternalFlag.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/ExternalFlag/ExternalFlagTask.h"

using namespace SourceXtractor;

struct ExternalFlagFixture {
  SimpleSource source;

  ExternalFlagFixture() {
    source.setProperty<DetectionFrameInfo>(4, 1, 1, 60000, 1e6, 1);
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ExternalFlag_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( external_flag_or_test, ExternalFlagFixture ) {
  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1, 0}, {2, 0}});

  auto flag_image = VectorImage<std::int64_t>::create(4, 1, std::vector<std::int64_t> {0, 1, 2, 3} );

  ExternalFlagTaskOr task({flag_image}, 0);
  task.computeProperties(source);

  auto& external_flag_property = source.getProperty<ExternalFlag>();

  BOOST_CHECK_EQUAL(external_flag_property.getFlag(), 3);
  BOOST_CHECK_EQUAL(external_flag_property.getCount(), 2);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( external_flag_and_test, ExternalFlagFixture ) {
  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1, 0}, {3, 0}});

  auto flag_image = VectorImage<std::int64_t>::create(4, 1, std::vector<std::int64_t> {0, 1, 2, 3} );

  ExternalFlagTaskAnd task({flag_image}, 0);
  task.computeProperties(source);

  auto& external_flag_property = source.getProperty<ExternalFlag>();

  BOOST_CHECK_EQUAL(external_flag_property.getFlag(), 1);
  BOOST_CHECK_EQUAL(external_flag_property.getCount(), 2);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( external_flag_min_test, ExternalFlagFixture ) {
  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1, 0}, {3, 0}});

  auto flag_image = VectorImage<std::int64_t>::create(4, 1, std::vector<std::int64_t> {0, 1, 2, 3} );

  ExternalFlagTaskMin task({flag_image}, 0);
  task.computeProperties(source);

  auto& external_flag_property = source.getProperty<ExternalFlag>();

  BOOST_CHECK_EQUAL(external_flag_property.getFlag(), 1);
  BOOST_CHECK_EQUAL(external_flag_property.getCount(), 1);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( external_flag_max_test, ExternalFlagFixture ) {
  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1, 0}, {3, 0}});

  auto flag_image = VectorImage<std::int64_t>::create(4, 1, std::vector<std::int64_t> {0, 1, 2, 3} );

  ExternalFlagTaskMax task({flag_image}, 0);
  task.computeProperties(source);

  auto& external_flag_property = source.getProperty<ExternalFlag>();

  BOOST_CHECK_EQUAL(external_flag_property.getFlag(), 3);
  BOOST_CHECK_EQUAL(external_flag_property.getCount(), 1);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( external_flag_most_test, ExternalFlagFixture ) {
  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1, 0}, {2, 0}, {3, 0}});

  auto flag_image = VectorImage<std::int64_t>::create(4, 1, std::vector<std::int64_t> {0, 1, 1, 3} );

  ExternalFlagTaskMost task({flag_image}, 0);
  task.computeProperties(source);

  auto& external_flag_property = source.getProperty<ExternalFlag>();

  BOOST_CHECK_EQUAL(external_flag_property.getFlag(), 1);
  BOOST_CHECK_EQUAL(external_flag_property.getCount(), 2);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
