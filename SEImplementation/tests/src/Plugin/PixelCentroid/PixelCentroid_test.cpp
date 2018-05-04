/**
 * @file tests/src/PixelCentroid_test.cpp
 * @date 05/27/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroidTask.h"

using namespace SExtractor;

struct PixelCentroidFixture {
  SimpleSource source;
  PixelCentroidTask pixel_centroid_task;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PixelCentroid_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( one_pixel_test, PixelCentroidFixture ) {
  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{0,0}});
  source.setProperty<PixelBoundaries>(0,0,0,0);
  source.setProperty<DetectionFramePixelValues>(
      std::vector<DetectionImage::PixelType>(),
      std::vector<DetectionImage::PixelType>( { 1.0 } ),
      std::vector<WeightImage::PixelType>( { 0.01 } ));

  pixel_centroid_task.computeProperties(source);

  auto centroid = source.getProperty<PixelCentroid>();
  BOOST_CHECK_CLOSE(centroid.getCentroidX(), 0.0, 0.00001);
  BOOST_CHECK_CLOSE(centroid.getCentroidY(), 0.0, 0.00001);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( multiple_pixels_test, PixelCentroidFixture ) {
  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1,3}, {8,4}});
  source.setProperty<PixelBoundaries>(1,3,8,4);
  source.setProperty<DetectionFramePixelValues>(
      std::vector<DetectionImage::PixelType>(),
      std::vector<DetectionImage::PixelType>( { 6.0, 4.0 } ),
      std::vector<WeightImage::PixelType>( { 0.01, 0.01 } ));

  pixel_centroid_task.computeProperties(source);

  auto centroid = source.getProperty<PixelCentroid>();
  BOOST_CHECK_CLOSE(centroid.getCentroidX(), 3.8, 0.00001);
  BOOST_CHECK_CLOSE(centroid.getCentroidY(), 3.4, 0.00001);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


