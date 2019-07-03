/*
 * ScaledModelComponent_test.cpp
 *
 *  Created on: July 03, 2019
 *      Author: Alejandro Alvarez Ayllon
 */
#include <boost/test/unit_test.hpp>
#include <numeric>
#include "ModelFitting/Models/OnlySmooth.h"
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Models/ScaledModelComponent.h"
#include "ModelFitting/utils.h"
#include "SEUtils/TestUtils.h"
#include "TestHelper.h"

using namespace ModelFitting;
using namespace SExtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ScaledModelComponent_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (ScaledModelComponent_noop_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(3., 1., LinearModelComponent::Decay::X_AXIS);

  ManualParameter scale_x{1.}, scale_y{1.};
  ScaledModelComponent scaled_model_component(std::move(linear_model_component), scale_x, scale_y);
  auto target = raster(scaled_model_component, 5);

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

BOOST_AUTO_TEST_CASE (ScaledModelComponent_twice_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(3., 1., LinearModelComponent::Decay::X_AXIS);

  ManualParameter scale_x{2.}, scale_y{2.};
  ScaledModelComponent scaled_model_component(std::move(linear_model_component), scale_x, scale_y);

  auto target = raster(scaled_model_component, 5);

  // It is like a zoom, so 1 pixel distance (X) at ScaledModelComponent is a 0.5 pixel distance (X)
  // at the LinearModelComponent
  std::vector<float> expected {
    2.0, 2.5, 3.0, 2.5, 2.0,
    2.0, 2.5, 3.0, 2.5, 2.0,
    2.0, 2.5, 3.0, 2.5, 2.0,
    2.0, 2.5, 3.0, 2.5, 2.0,
    2.0, 2.5, 3.0, 2.5, 2.0,
  };

  BOOST_CHECK(compareCollections(expected, target));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (ScaledModelComponent_thrice_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(3., 1., LinearModelComponent::Decay::X_AXIS);

  ManualParameter scale_x{3.}, scale_y{3.};
  ScaledModelComponent scaled_model_component(std::move(linear_model_component), scale_x, scale_y);

  auto target = raster(scaled_model_component, 5);

  // It is like a zoom, so 1 pixel distance (X) at ScaledModelComponent is a 0.333 pixel distance (X)
  // at the LinearModelComponent
  std::vector<float> expected {
    2.333, 2.667, 3.0, 2.667, 2.333,
    2.333, 2.667, 3.0, 2.667, 2.333,
    2.333, 2.667, 3.0, 2.667, 2.333,
    2.333, 2.667, 3.0, 2.667, 2.333,
    2.333, 2.667, 3.0, 2.667, 2.333,
  };

  BOOST_CHECK(compareCollections(expected, target, 1e-8, 1e-3));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (ScaledModelComponent_twice_x_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(2., 1., LinearModelComponent::Decay::BOTH);

  ManualParameter scale_x{2.}, scale_y{1.};
  ScaledModelComponent scaled_model_component(std::move(linear_model_component), scale_x, scale_y);

  auto target = raster(scaled_model_component, 5);

  std::vector<float> expected {
    0.000, 0.000, 0.000, 0.000, 0.000,
    0.586, 0.882, 1.000, 0.882, 0.586,
    1.000, 1.500, 2.000, 1.500, 1.000,
    0.586, 0.882, 1.000, 0.882, 0.586,
    0.000, 0.000, 0.000, 0.000, 0.000,
  };

  BOOST_CHECK(compareCollections(expected, target, 1e-8, 1e-3));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

//-----------------------------------------------------------------------------
