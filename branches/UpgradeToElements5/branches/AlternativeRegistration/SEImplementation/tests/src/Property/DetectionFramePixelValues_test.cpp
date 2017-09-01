/**
 * @file tests/src/DetectionFramePixelValues_test.cpp
 * @date 05/27/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/Source.h"
#include "SEFramework/Task/TaskRegistry.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Property/PixelCoordinateList.h"

#include "SEImplementation/Property/DetectionFramePixelValues.h"
#include "SEImplementation/Task/DetectionFramePixelValuesTask.h"

using namespace SExtractor;

struct DetectionFramePixelValuesFixture {
  std::shared_ptr<TaskRegistry> task_registry;
  std::shared_ptr<Source> source;

  DetectionFramePixelValuesFixture() : task_registry(new TaskRegistry) {}
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (DetectionFramePixelValues_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(example_test, DetectionFramePixelValuesFixture) {
  source.reset(new Source(
      std::vector<PixelCoordinate>( { PixelCoordinate(2,0), PixelCoordinate(1,1) } ), task_registry));
  auto& pixel_list = source->getProperty<PixelCoordinateList>().getCoordinateList();
  BOOST_CHECK(pixel_list == std::vector<PixelCoordinate>( { PixelCoordinate(2,0), PixelCoordinate(1,1) } ));

  auto image = std::make_shared<VectorImage<double>>(3, 2, std::vector<double>{0.0, 1.0, 2.0, 3.0, 4.0, 5.0});

  DetectionFramePixelValuesTask task(image);
  task.computeProperties(*source);

  auto& pixel_values = source->getProperty<DetectionFramePixelValues>();
  BOOST_CHECK(pixel_values.getValues() == std::vector<double>( {2.0, 4.0} ));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


