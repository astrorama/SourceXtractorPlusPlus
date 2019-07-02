/*
 * SersicProfile_test.cpp
 *
 *  Created on: July 01, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Models/SersicProfile.h"
#include "SEUtils/SEUtils/IsClose.h"

using namespace ModelFitting;
using namespace SExtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SersicProfile_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (SersicProfile_test) {
  ManualParameter exp_i0 {12.};
  ManualParameter exp_n {1.};
  ManualParameter exp_k {1.};
  SersicProfile profile(exp_i0, exp_n, exp_k);
  BOOST_CHECK(isClose(profile(10.), 0.0005447991571498182));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (SersicProfile_update_test) {
  ManualParameter exp_i0 {12.};
  ManualParameter exp_n {1.};
  ManualParameter exp_k {1.};
  SersicProfile profile(exp_i0, exp_n, exp_k);

  exp_i0.setValue(60);

  BOOST_CHECK(isClose(profile(10.), 0.002723995785749091));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

//-----------------------------------------------------------------------------
