/*
 * AperturePhotometry_test.cpp
 *
 *  Created on: Oct 19, 2016
 *      Author: mschefer
 */

#include <string>
#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Aperture/CircularAperture.h"
#include "SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Plugin/SourceFlags/SourceFlags.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlagTask.h"
#include "SEImplementation/Plugin/AperturePhotometry/ApertureFlag.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"

using namespace SExtractor;

struct AperturePhotometryFixture {
  SimpleSource source;
  std::map<std::string, std::vector<unsigned>> images_per_group;

  AperturePhotometryFixture() {
    images_per_group.emplace("r", std::vector<unsigned>{0});
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PixelCentroid_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( one_pixel_test, AperturePhotometryFixture ) {
  source.setIndexedProperty<PixelCentroid>(0, 0, 0);

  auto image = VectorImage<MeasurementImage::PixelType>::create(1, 1);
  image->setValue(0, 0, 1);

  auto frame = std::make_shared<MeasurementImageFrame>(image);
  source.setIndexedProperty<DetectionFrame>(0, frame);
  source.setIndexedProperty<MeasurementFrame>(0, frame);
  source.setIndexedProperty<PixelBoundaries>(0, 0, 0, 0, 0);
  source.setIndexedProperty<MeasurementFramePixelCentroid>(0, 0, 0);
  source.setIndexedProperty<PixelCoordinateList>(0, PixelCoordinateList{{{0, 0}}});
  source.setIndexedProperty<JacobianSource>(0);

  ApertureFlagTask aperture_flag_task(std::make_shared<CircularAperture>(.5), images_per_group, 0);
  AperturePhotometryTask aperture_photometry_task(std::make_shared<CircularAperture>(.5), 0, 0, 0, false);
  aperture_flag_task.computeProperties(source);
  aperture_photometry_task.computeProperties(source);

  auto aperture_photometry = source.getProperty<AperturePhotometry>();
  BOOST_CHECK_CLOSE(aperture_photometry.getFlux(), 3.14159 * .25, 10);

  // It is on a boundary
  auto aperture_flag = source.getProperty<ApertureFlag>();
  BOOST_CHECK(aperture_flag.getFlags()[0] == SourceFlags::BOUNDARY);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( neighbour_test, AperturePhotometryFixture ) {
  auto image = VectorImage<MeasurementImage::PixelType>::create(3, 3, std::vector<MeasurementImage::PixelType>{
    0, 0, 5,
    0, 1, 0,
    0, 0, 0
  });

  auto frame = std::make_shared<MeasurementImageFrame>(image);
  source.setIndexedProperty<DetectionFrame>(0, frame);
  source.setIndexedProperty<MeasurementFrame>(0, frame);
  source.setIndexedProperty<PixelBoundaries>(0, 0, 0, 2, 2);
  source.setIndexedProperty<PixelCoordinateList>(0, PixelCoordinateList{{
    {0, 0}, {1, 0},
    {0, 1}, {1, 1}, {2, 1},
    {0, 2}, {2, 2}, {2, 2}
  }});
  source.setIndexedProperty<PixelCentroid>(0, 1, 1);
  source.setIndexedProperty<MeasurementFramePixelCentroid>(0, 1, 1);
  source.setIndexedProperty<JacobianSource>(0);

  ApertureFlagTask aperture_flag_task(std::make_shared<CircularAperture>(1.), images_per_group, 0);
  AperturePhotometryTask aperture_photometry_task(std::make_shared<CircularAperture>(1.0), 0, 0, 0, false);
  aperture_flag_task.computeProperties(source);
  aperture_photometry_task.computeProperties(source);

  auto aperture_photometry = source.getProperty<AperturePhotometry>();
  BOOST_CHECK_CLOSE(aperture_photometry.getFlux(), 1.45, 10);
  // There is one pixel that belongs to a neighbour
  auto aperture_flag = source.getProperty<ApertureFlag>();
  BOOST_CHECK(aperture_flag.getFlags()[0] & SourceFlags::BLENDED);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

