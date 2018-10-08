/*
 * AperturePhotometry_test.cpp
 *
 *  Created on: Oct 19, 2016
 *      Author: mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Aperture/CircularAperture.h"
#include "SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/NeighbourInfo/NeighbourInfo.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"

using namespace SExtractor;

struct AperturePhotometryFixture {
  SimpleSource source;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PixelCentroid_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( one_pixel_test, AperturePhotometryFixture ) {
  source.setIndexedProperty<PixelCentroid>(0, 0, 0);

  auto image = VectorImage<MeasurementImage::PixelType>::create(1, 1);
  image->setValue(0, 0, 1);

  auto neighbour = VectorImage<int>::create(1, 1);
  source.setIndexedProperty<NeighbourInfo>(0, PixelCoordinate{0,0}, neighbour);

  auto frame = std::make_shared<MeasurementImageFrame>(image);
  source.setIndexedProperty<DetectionFrame>(0, frame);
  source.setIndexedProperty<PixelBoundaries>(0, 0, 0, 0, 0);
  source.setIndexedProperty<PixelCoordinateList>(0, PixelCoordinateList{{{0, 0}}});

  AperturePhotometryTask aperture_photometry_task(std::make_shared<CircularAperture>(.5), 0, 0, 0, false);
  aperture_photometry_task.computeProperties(source);

  auto aperture_photometry = source.getProperty<AperturePhotometry>();
  BOOST_CHECK_CLOSE(aperture_photometry.getFlux(), 3.14159 * .25, 10);
  // It is on a boundary
  BOOST_CHECK(aperture_photometry.getFlag() == 0x08);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( neighbour_test, AperturePhotometryFixture ) {
  auto image = VectorImage<MeasurementImage::PixelType>::create(3, 3, std::vector<MeasurementImage::PixelType>{
    0, 0, 0,
    0, 1, 0,
    0, 0, 0
  });
  auto neighbour = VectorImage<int>::create(3, 3, std::vector<int>{
    0, 0, 1,
    0, 0, 0,
    0, 0, 0,
  });

  auto frame = std::make_shared<MeasurementImageFrame>(image);
  source.setIndexedProperty<DetectionFrame>(0, frame);
  source.setIndexedProperty<PixelBoundaries>(0, 0, 0, 2, 2);
  source.setIndexedProperty<PixelCoordinateList>(0, PixelCoordinateList{{
    {0, 0}, {1, 0}, {2, 0},
    {0, 1}, {1, 1}, {2, 1},
    {0, 2}, {2, 2}, {2, 2}
  }});
  source.setIndexedProperty<PixelCentroid>(0, 1, 1);
  source.setIndexedProperty<NeighbourInfo>(0, PixelCoordinate{0,0}, neighbour);


  AperturePhotometryTask aperture_photometry_task(std::make_shared<CircularAperture>(1.0), 0, 0, 0, false);
  aperture_photometry_task.computeProperties(source);

  auto aperture_photometry = source.getProperty<AperturePhotometry>();
  BOOST_CHECK_CLOSE(aperture_photometry.getFlux(), 1., 10);
  // There is one pixel that belongs to a neighbour
  BOOST_CHECK(aperture_photometry.getFlag() & 0x02);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

