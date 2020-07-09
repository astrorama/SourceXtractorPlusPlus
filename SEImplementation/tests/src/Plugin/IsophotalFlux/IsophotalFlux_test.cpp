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
/*
 * IsophotalFlux_test.cpp
 *
 *  Created on: Oct 19, 2016
 *      Author: mschefer
 */

#include <boost/test/unit_test.hpp>
#include <limits>

#include "SEFramework/Property/DetectionFrame.h"
#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Image/ConstantImage.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFrameInfo/DetectionFrameInfo.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFluxTask.h"

using namespace SourceXtractor;

struct IsophotalFluxFixture {
  SimpleSource source;
  IsophotalFluxTask isophotal_flux_task{0};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PixelCentroid_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(one_pixel_test, IsophotalFluxFixture) {
  source.setProperty<DetectionFrame>(
    std::make_shared<DetectionImageFrame>(ConstantImage<SeFloat>::create(1, 1, 0),
                                          nullptr, 1e6, nullptr, 1, 1, 0));
  source.setProperty<DetectionFramePixelValues>(
    std::vector<DetectionImage::PixelType>({1.0}),
    std::vector<DetectionImage::PixelType>(),
    std::vector<WeightImage::PixelType>({0.01}));

  source.setProperty<DetectionFrameInfo>(1, 1, 1, 65000, 1e6, 1);


  isophotal_flux_task.computeProperties(source);

  auto isophotal_flux = source.getProperty<IsophotalFlux>();
  BOOST_CHECK_CLOSE(isophotal_flux.getFlux(), 1.0, 0.0001);
  BOOST_CHECK_CLOSE(isophotal_flux.getFluxError(), sqrt(.01 + 1), 0.0001);
}

BOOST_FIXTURE_TEST_CASE(isophotal_test, IsophotalFluxFixture) {
  source.setProperty<DetectionFrame>(
    std::make_shared<DetectionImageFrame>(ConstantImage<SeFloat>::create(1, 1, 0),
                                          nullptr, 1e6, nullptr, 1, 1, 0));
  source.setProperty<DetectionFramePixelValues>(
    std::vector<DetectionImage::PixelType>({1.0, 3.0, 5.7}),
    std::vector<DetectionImage::PixelType>(),
    std::vector<WeightImage::PixelType>({0.01, 0.01, 0.01}));

  source.setProperty<DetectionFrameInfo>(1, 1, 1, 65000, 1e6, 1);

  isophotal_flux_task.computeProperties(source);

  auto isophotal_flux = source.getProperty<IsophotalFlux>();
  BOOST_CHECK_CLOSE(isophotal_flux.getFlux(), 9.7, 0.0001);
  BOOST_CHECK_CLOSE(isophotal_flux.getFluxError(), sqrt(.03 + 9.7), 0.0001);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


