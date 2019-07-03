/*
 * RotatedModelComponent_test.cpp
 *
 *  Created on: July 03, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include <numeric>
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Models/RotatedModelComponent.h"
#include "ModelFitting/utils.h"
#include "SEUtils/TestUtils.h"
#include "TestHelper.h"

using namespace ModelFitting;
using namespace SExtractor;


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (RotatedModelComponent_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (RotatedModelComponent_noop_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(3., 1., LinearModelComponent::Decay::X_AXIS);

  ManualParameter rotation{0.};
  RotatedModelComponent rotated_model_component(std::move(linear_model_component), rotation);
  auto target = raster(rotated_model_component, 5);

  std::vector<float> expected {
    1, 2, 3, 2, 1,
    1, 2, 3, 2, 1,
    1, 2, 3, 2, 1,
    1, 2, 3, 2, 1,
    1, 2, 3, 2, 1,
  };

  BOOST_CHECK(compareCollections(expected, target));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (RotatedModelComponent_90_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(3., 1., LinearModelComponent::Decay::X_AXIS);

  ManualParameter rotation{M_PI / 2};
  RotatedModelComponent rotated_model_component(std::move(linear_model_component), rotation);
  auto target = raster(rotated_model_component, 5);

  std::vector<float> expected {
    1, 1, 1, 1, 1,
    2, 2, 2, 2, 2,
    3, 3, 3, 3, 3,
    2, 2, 2, 2, 2,
    1, 1, 1, 1, 1,
  };

  BOOST_CHECK(compareCollections(expected, target));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (RotatedModelComponent_45_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(3., 1., LinearModelComponent::Decay::X_AXIS);

  ManualParameter rotation{M_PI / 4};
  RotatedModelComponent rotated_model_component(std::move(linear_model_component), rotation);
  auto target = raster(rotated_model_component, 5);

  std::vector<float> expected {
    3.000, 2.293, 1.586, 0.879, 0.172,
    2.293, 3.000, 2.293, 1.586, 0.879,
    1.586, 2.293, 3.000, 2.293, 1.586,
    0.879, 1.586, 2.293, 3.000, 2.293,
    0.172, 0.879, 1.586, 2.293, 3.000,
  };

  BOOST_CHECK(compareCollections(expected, target, 1e-8, 1e-2));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (RotatedModelComponent_45_circular_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(2., 1., LinearModelComponent::Decay::BOTH);
  auto reference = raster(*linear_model_component, 5);

  std::vector<float> expected {
    0.000, 0.000, 0.000, 0.000, 0.000,
    0.000, 0.586, 1.000, 0.586, 0.000,
    0.000, 1.000, 2.000, 1.000, 0.000,
    0.000, 0.586, 1.000, 0.586, 0.000,
    0.000, 0.000, 0.000, 0.000, 0.000,
  };

  BOOST_CHECK(compareCollections(expected, reference, 1e-8, 1e-2));

  ManualParameter rotation{M_PI / 4};
  RotatedModelComponent rotated_model_component(std::move(linear_model_component), rotation);
  auto target = raster(rotated_model_component, 5);

  BOOST_CHECK(compareCollections(expected, target, 1e-8, 1e-2));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

//-----------------------------------------------------------------------------
