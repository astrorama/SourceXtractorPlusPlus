/**
 * @file tests/src/PixelCentroid_test.cpp
 * @date 05/27/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/Source.h"
#include "SEFramework/Task/TaskRegistry.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Property/DetectionFramePixelValues.h"
#include "SEImplementation/Property/PixelCentroid.h"
#include "SEImplementation/Task/PixelCentroidTaskFactory.h"

using namespace SExtractor;

struct PixelCentroidFixture {
  std::shared_ptr<TaskRegistry> task_registry;
  std::shared_ptr<Source> source;

  PixelCentroidFixture() : task_registry(new TaskRegistry) {
    task_registry->registerTaskFactory(std::unique_ptr<TaskFactory>(new PixelCentroidTaskFactory));
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PixelCentroid_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( one_pixel_test, PixelCentroidFixture ) {
  source.reset(new Source(task_registry));
  source->setProperty<PixelCoordinateList>(std::unique_ptr<PixelCoordinateList>{new PixelCoordinateList{
    {PixelCoordinate(0,0)}
  }});

  auto& pixel_list = source->getProperty<PixelCoordinateList>().getCoordinateList();
  BOOST_CHECK(bool(pixel_list == std::vector<PixelCoordinate>( { PixelCoordinate(0,0) } )));

  source->setProperty(std::unique_ptr<DetectionFramePixelValues>(
      new DetectionFramePixelValues(std::vector<double>( { 1.0 } ))));
  BOOST_CHECK(source->getProperty<DetectionFramePixelValues>().getValues() == std::vector<double>({1.0}));

  auto centroid = source->getProperty<PixelCentroid>();
  BOOST_CHECK_CLOSE(centroid.getCentroidX(), 0.0, 0.000001);
  BOOST_CHECK_CLOSE(centroid.getCentroidY(), 0.0, 0.000001);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( multiple_pixels_test, PixelCentroidFixture ) {
  source.reset(new Source(task_registry));
  source->setProperty<PixelCoordinateList>(std::unique_ptr<PixelCoordinateList>{new PixelCoordinateList{
    {PixelCoordinate(1,3), PixelCoordinate(8,4)}
  }});
  auto& pixel_list = source->getProperty<PixelCoordinateList>().getCoordinateList();
  BOOST_CHECK(pixel_list == std::vector<PixelCoordinate>( { PixelCoordinate(1, 3), PixelCoordinate(8, 4) } ));

  source->setProperty(std::unique_ptr<DetectionFramePixelValues>(
      new DetectionFramePixelValues(std::vector<double>( { 6.0, 4.0 } ))));
  BOOST_CHECK(source->getProperty<DetectionFramePixelValues>().getValues() == std::vector<double>( { 6.0, 4.0 } ));

  auto centroid = source->getProperty<PixelCentroid>();
  BOOST_CHECK_CLOSE(centroid.getCentroidX(), 3.8, 0.000001);
  BOOST_CHECK_CLOSE(centroid.getCentroidY(), 3.4, 0.000001);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


