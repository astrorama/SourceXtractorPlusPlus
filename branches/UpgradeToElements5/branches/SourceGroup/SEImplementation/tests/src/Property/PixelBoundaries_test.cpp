/**
 * @file tests/src/PixelBoundaries_test.cpp
 * @date 05/30/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/Source.h"
#include "SEFramework/Task/TaskProvider.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Property/PixelBoundaries.h"
#include "SEImplementation/Task/PixelBoundariesTaskFactory.h"

using namespace SExtractor;

struct PixelBoundariesFixture {
  std::shared_ptr<TaskFactoryRegistry> task_factory_registry;
  std::shared_ptr<TaskProvider> task_provider;
  std::shared_ptr<Source> source;

  PixelBoundariesFixture() :
      task_factory_registry(new TaskFactoryRegistry()),
      task_provider(new TaskProvider(task_factory_registry)) {
    task_factory_registry->registerTaskFactory<PixelBoundaries>(std::unique_ptr<TaskFactory>(new PixelBoundariesTaskFactory));
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PixelBoundaries_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( boundaries_test, PixelBoundariesFixture ) {
  source.reset(new Source(task_provider));
  source->setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1,3}, {8,-4}});
  auto& pixel_list = source->getProperty<PixelCoordinateList>().getCoordinateList();
  BOOST_CHECK(pixel_list == std::vector<PixelCoordinate>( { PixelCoordinate(1, 3), PixelCoordinate(8, -4) } ));

  auto boundaries = source->getProperty<PixelBoundaries>();
  BOOST_CHECK(boundaries.getMin() == PixelCoordinate(1, -4));
  BOOST_CHECK(boundaries.getMax() == PixelCoordinate(8, 3));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


