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
 * SersicProfile_test.cpp
 *
 *  Created on: July 01, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Models/SersicProfile.h"
#include "SEUtils/IsClose.h"

using namespace ModelFitting;
using namespace SourceXtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SersicProfile_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (SersicProfile_test) {
  auto exp_i0 = std::make_shared<ManualParameter>(12.);
  auto exp_n = std::make_shared<ManualParameter>(1.);
  auto exp_k = std::make_shared<ManualParameter>(1.);
  SersicProfile profile(exp_i0, exp_n, exp_k);
  BOOST_CHECK(isClose(profile(10.), 0.0005447991571498182));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (SersicProfile_update_test) {
  auto exp_i0 = std::make_shared<ManualParameter>(12.);
  auto exp_n = std::make_shared<ManualParameter>(1.);
  auto exp_k = std::make_shared<ManualParameter>(1.);
  SersicProfile profile(exp_i0, exp_n, exp_k);

  exp_i0->setValue(60);

  BOOST_CHECK(isClose(profile(10.), 0.002723995785749091));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

//-----------------------------------------------------------------------------
