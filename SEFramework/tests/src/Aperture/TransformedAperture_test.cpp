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
//
// Created by aalvarez on 10/8/18.
//

#include <boost/test/unit_test.hpp>
#include "SEFramework/Aperture/CircularAperture.h"
#include "SEFramework/Aperture/TransformedAperture.h"

using namespace SourceXtractor;


BOOST_AUTO_TEST_SUITE (Aperture_test)

BOOST_AUTO_TEST_CASE(Identity_test) {
  auto original = std::make_shared<CircularAperture>(2.);
  auto transformed = std::make_shared<TransformedAperture>(original, std::make_tuple(
    1., 0.,
    0., 1.
    ));

  auto omin = original->getMinPixel(10, 10);
  auto tmin = transformed->getMinPixel(10, 10);
  auto omax = original->getMaxPixel(10, 10);
  auto tmax = transformed->getMaxPixel(10, 10);

  BOOST_CHECK_EQUAL(omin.m_x, tmin.m_x);
  BOOST_CHECK_EQUAL(omin.m_y, tmin.m_y);
  BOOST_CHECK_EQUAL(omax.m_x, tmax.m_x);
  BOOST_CHECK_EQUAL(omax.m_y, tmax.m_y);
  BOOST_CHECK_EQUAL(original->getArea(10, 10, 10, 10), transformed->getArea(10, 10, 10, 10));
  BOOST_CHECK_EQUAL(original->getRadiusSquared(10, 10, 10, 10), transformed->getRadiusSquared(10, 10, 10, 10));
}

BOOST_AUTO_TEST_CASE(Scale_test) {
  auto original = std::make_shared<CircularAperture>(2.);
  auto transformed = std::make_shared<TransformedAperture>(original, std::make_tuple(
    2., 0.,
    0., 2.
    ));

  auto omin = original->getMinPixel(10, 10);
  auto tmin = transformed->getMinPixel(10, 10);
  auto omax = original->getMaxPixel(10, 10);
  auto tmax = transformed->getMaxPixel(10, 10);

  BOOST_CHECK_EQUAL(2 * (10 - omin.m_x), 10 - tmin.m_x);
  BOOST_CHECK_EQUAL(2 * (10 - omin.m_y), 10 - tmin.m_y);
  BOOST_CHECK_EQUAL(2 * (omax.m_x - 10), tmax.m_x - 10);
  BOOST_CHECK_EQUAL(2 * (omax.m_y - 10), tmax.m_y - 10);

  BOOST_CHECK_EQUAL(original->getArea(10, 10, 10, 10), transformed->getArea(10, 10, 10, 10));
  BOOST_CHECK_LT(original->getArea(10, 10, 7, 7), transformed->getArea(10, 10, 7, 7));
}

BOOST_AUTO_TEST_CASE(Shear_test) {
  auto original = std::make_shared<CircularAperture>(5.);
  auto transformed = std::make_shared<TransformedAperture>(original, std::make_tuple(
    1., 1.,
    0., 1.
  ));

  auto omin = original->getMinPixel(10, 10);
  auto tmin = transformed->getMinPixel(10, 10);
  auto omax = original->getMaxPixel(10, 10);
  auto tmax = transformed->getMaxPixel(10, 10);

  BOOST_CHECK_EQUAL(10 - omin.m_x, 10 - tmin.m_x);
  BOOST_CHECK_LT(10 - omin.m_y, 10 - tmin.m_y);
  BOOST_CHECK_EQUAL(omax.m_x - 10, tmax.m_x - 10);
  BOOST_CHECK_LT(omax.m_y - 10, tmax.m_y - 10);

  BOOST_CHECK_GT(original->getArea(10, 10, 9, 15), transformed->getArea(10, 10, 9, 15));
}

BOOST_AUTO_TEST_SUITE_END ()
