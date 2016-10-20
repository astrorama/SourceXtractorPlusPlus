/*
 * IsophotalFlux_test.cpp
 *
 *  Created on: Oct 19, 2016
 *      Author: mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFluxTask.h"

using namespace SExtractor;

struct IsophotalFluxFixture {
  SimpleSource source;
  IsophotalFluxTask isophotal_flux_task {0};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PixelCentroid_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( one_pixel_test, IsophotalFluxFixture ) {
  source.setProperty<DetectionFramePixelValues>(std::vector<DetectionImage::PixelType>( { 1.0 } ));

  isophotal_flux_task.computeProperties(source);

  auto isophotal_flux = source.getProperty<IsophotalFlux>();
  BOOST_CHECK_CLOSE(isophotal_flux.getFlux(), 1.0, 0.0001);
}

BOOST_FIXTURE_TEST_CASE( isophotal_test, IsophotalFluxFixture ) {
  source.setProperty<DetectionFramePixelValues>(std::vector<DetectionImage::PixelType>( { 1.0, 3.0, 5.7 } ));

  isophotal_flux_task.computeProperties(source);

  auto isophotal_flux = source.getProperty<IsophotalFlux>();
  BOOST_CHECK_CLOSE(isophotal_flux.getFlux(), 9.7, 0.0001);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


