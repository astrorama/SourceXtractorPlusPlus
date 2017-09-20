/*
 * AperturePhotometry_test.cpp
 *
 *  Created on: Oct 19, 2016
 *      Author: mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"

using namespace SExtractor;

struct AperturePhotometryFixture {
  SimpleSource source;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PixelCentroid_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( one_pixel_test, AperturePhotometryFixture ) {
  source.setIndexedProperty<MeasurementFramePixelCentroid>(0, 0, 0);

  auto image = VectorImage<MeasurementImage::PixelType>::create(1, 1);
  image->setValue(0, 0, 1);
  source.setIndexedProperty<MeasurementFrame>(0, std::make_shared<DetectionImageFrame>(image));

  AperturePhotometryTask aperture_photometry_task(std::make_shared<CircularAperture>(.5), 0, 0, 0, false);
  aperture_photometry_task.computeProperties(source);

  auto aperture_photometry = source.getProperty<AperturePhotometry>();
  BOOST_CHECK_CLOSE(aperture_photometry.getFlux(), 3.14159 * .25, 10);
}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

