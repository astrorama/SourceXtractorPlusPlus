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
/**
 * @file ManualParameter_test.cpp
 *
 * Created on: January 9, 2015
 *     Author: Pierre Dubath
 */

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "ElementsKernel/Exception.h"
// class under test
#include "ModelFitting/Parameters/ManualParameter.h"

using namespace ModelFitting;
using namespace std;

// tolerance value to compare floating point numbers
double tolerance = 1e-12;

BOOST_AUTO_TEST_SUITE (ManualParameter_test)

BOOST_AUTO_TEST_CASE(WithoutFixture) {
  ManualParameter mp {3.0};
  BOOST_CHECK_EQUAL(3.0, mp.getValue());
}

BOOST_AUTO_TEST_SUITE_END ()

