/*
 * AperturePhotometry_test.cpp
 *
 *  Created on: Oct 19, 2016
 *      Author: mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometry.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/AperturePhotometry/AperturePhotometryTask.h"

using namespace SExtractor;

struct AperturePhotometryFixture {
  SimpleSource source;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PixelCentroid_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( one_pixel_test, AperturePhotometryFixture ) {
  source.setProperty<PixelCentroid>(0, 0);

  auto image = std::make_shared<VectorImage<DetectionImage::PixelType>>(1, 1);
  image->setValue(0, 0, 1);
  source.setProperty<DetectionFrame>(image);

  AperturePhotometryTask aperture_photometry_task(std::make_shared<CircularAperture>(.5), 0, 0);
  aperture_photometry_task.computeProperties(source);

  auto aperture_photometry = source.getProperty<AperturePhotometry>();
  BOOST_CHECK_CLOSE(aperture_photometry.getFlux(), 3.14159 * .25, 10);
}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

