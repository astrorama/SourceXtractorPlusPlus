/*
 * TransformModelComponent_test.cpp
 *
 *  Created on: July 03, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include <numeric>
#include "ModelFitting/Models/OnlySmooth.h"
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Models/TransformModelComponent.h"
#include "ModelFitting/utils.h"
#include "SEUtils/TestUtils.h"
#include "TestHelper.h"

using namespace ModelFitting;
using namespace SExtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (TransformModelComponent_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (TransformModelComponent_noop_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(3., 1., LinearModelComponent::Decay::X_AXIS);

  std::tuple<double, double, double, double> transform{1., 0.,
                                                       0., 1.};
  TransformModelComponent transform_model_component(std::move(linear_model_component), transform);
  auto target = raster(transform_model_component, 5);

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

BOOST_AUTO_TEST_CASE (TransformModelComponent_scale_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(3., 1., LinearModelComponent::Decay::X_AXIS);
  auto reference = raster(*linear_model_component, 5);

  std::tuple<double, double, double, double> transform{2., 0.,
                                                       0., 2.};
  TransformModelComponent transform_model_component(std::move(linear_model_component), transform);
  auto target = raster(transform_model_component, 5);

  // Note than, unlike ScaledModelComponent, TransformModelComponent takes into account the new area.
  // This is, each pixel now is of size 4*4, so its integrated value is adjusted accordingly.
  std::vector<float> expected {
    0.500, 0.625, 0.75, 0.625, 0.500,
    0.500, 0.625, 0.75, 0.625, 0.500,
    0.500, 0.625, 0.75, 0.625, 0.500,
    0.500, 0.625, 0.75, 0.625, 0.500,
    0.500, 0.625, 0.75, 0.625, 0.500,
  };

  BOOST_CHECK(compareCollections(expected, target));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (TransformModelComponent_sharp_scale_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(3., 1., LinearModelComponent::Decay::X_AXIS, 3.);
  auto reference = raster(*linear_model_component, 5);

  std::tuple<double, double, double, double> transform{2., 0.,
                                                       0., 2.};
  TransformModelComponent transform_model_component(std::move(linear_model_component), transform);
  transform_model_component.updateRasterizationInfo(1., 10.);
  auto target = raster(transform_model_component, 5);

  // Note than, unlike ScaledModelComponent, TransformModelComponent takes into account the new area.
  // This is, each pixel now is of size 4*4, so its integrated value is adjusted accordingly.
  std::vector<float> expected {
    0.500, 0.625, 0.75, 0.625, 0.500,
    0.500, 0.625, 0.75, 0.625, 0.500,
    0.500, 0.625, 0.75, 0.625, 0.500,
    0.500, 0.625, 0.75, 0.625, 0.500,
    0.500, 0.625, 0.75, 0.625, 0.500,
  };

  BOOST_CHECK(compareCollections(expected, target));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (TransformModelComponent_scale_integral_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(2., 1., LinearModelComponent::Decay::BOTH);
  auto reference_raster = raster(*linear_model_component, 9);

  std::tuple<double, double, double, double> transform{2., 0.,
                                                       0., 2.};
  TransformModelComponent transform_model_component(std::move(linear_model_component), transform);
  auto target_raster = raster(transform_model_component, 9);

  // The integral is kept through the transformation (with some tolerance!)
  double integral_scaled = std::accumulate(target_raster.begin(), target_raster.end(), 0.);
  double integral_original = std::accumulate(reference_raster.begin(), reference_raster.end(), 0.);
  BOOST_CHECK(checkIsClose(integral_original, integral_scaled, 1e-8, 1e-2));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (TransformModelComponent_sharp_scale_integral_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(2., 1., LinearModelComponent::Decay::BOTH, 3);
  auto reference_raster = raster(*linear_model_component, 9);

  std::tuple<double, double, double, double> transform{2., 0.,
                                                       0., 2.};
  TransformModelComponent transform_model_component(std::move(linear_model_component), transform);
  auto target_raster = raster(transform_model_component, 9);

  // The integral is kept through the transformation (with some tolerance!)
  double integral_scaled = std::accumulate(target_raster.begin(), target_raster.end(), 0.);
  double integral_original = std::accumulate(reference_raster.begin(), reference_raster.end(), 0.);
  BOOST_CHECK(checkIsClose(integral_original, integral_scaled, 1e-8, 1e-2));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (TransformModelComponent_rotate_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(3., 1., LinearModelComponent::Decay::X_AXIS);

  double angle = M_PI / 4;
  std::tuple<double, double, double, double> transform{std::cos(angle), -std::sin(angle),
                                                       std::sin(angle), std::cos(angle)};
  TransformModelComponent transform_model_component(std::move(linear_model_component), transform);
  auto target = raster(transform_model_component, 5);

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

BOOST_AUTO_TEST_CASE (TransformModelComponent_rotate_circular) {
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

  double angle = M_PI / 4;
  std::tuple<double, double, double, double> transform{std::cos(angle), -std::sin(angle),
                                                       std::sin(angle), std::cos(angle)};
  TransformModelComponent transform_model_component(std::move(linear_model_component), transform);
  auto target = raster(transform_model_component, 5);

  BOOST_CHECK(compareCollections(expected, target, 1e-8, 1e-2));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (TransformModelComponent_sharp_rotate_circular) {
  auto linear_model_component = make_unique<LinearModelComponent>(2., 1., LinearModelComponent::Decay::BOTH, 3.);
  auto reference = raster(*linear_model_component, 5);

  // In this case, it is the sample what rotates
  std::vector<float> expected {
    0.000, 0.000, 0.000, 0.000, 0.000,
    0.000, 1.000, 0.586, 1.000, 0.000,
    0.000, 0.586, 2.000, 0.586, 0.000,
    0.000, 1.000, 0.586, 1.000, 0.000,
    0.000, 0.000, 0.000, 0.000, 0.000,
  };

  double angle = M_PI / 4;
  std::tuple<double, double, double, double> transform{std::cos(angle), -std::sin(angle),
                                                       std::sin(angle), std::cos(angle)};
  TransformModelComponent transform_model_component(std::move(linear_model_component), transform);
  auto target = raster(transform_model_component, 5);

  BOOST_CHECK(compareCollections(expected, target, 1e-8, 1e-2));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (TransformModelComponent_shear_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(3., 1., LinearModelComponent::Decay::BOTH);

  std::tuple<double, double, double, double> transform{1., -1.,
                                                       0.,  1.};
  TransformModelComponent shear_model_component(std::move(linear_model_component), transform);
  auto target = raster(shear_model_component, 5);

  std::vector<float> expected {
    0.000, 0.000, 1.000, 1.586, 1.000,
    0.000, 0.764, 2.000, 2.000, 0.764,
    0.172, 1.586, 3.000, 1.586, 0.172,
    0.764, 2.000, 2.000, 0.764, 0.000,
    1.000, 1.586, 1.000, 0.000, 0.000,
  };
  /*
  Note that the test fails, and, unlike the sharp shear tests, the result given is
  this one:
  std::vector<float> expected {
    1.000, 0.764, 0.172, 0.000, 0.000,
    1.586, 2.000, 1.586, 0.764, 0.000,
    1.000, 2.000, 3.000, 2.000, 1.000,
    0.000, 0.764, 1.586, 2.000, 1.586,
    0.000, 0.000, 0.172, 0.764, 1.000
  };
  It looks like the shear is affecting a different axis
  */

  BOOST_CHECK(compareCollections(expected, target, 1e-8, 1e-2));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (TransformModelComponent_sharp_shear_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(3., 1., LinearModelComponent::Decay::BOTH, 9.);

  std::tuple<double, double, double, double> transform{1., -1.,
                                                       0.,  1.};
  TransformModelComponent shear_model_component(std::move(linear_model_component), transform);
  shear_model_component.updateRasterizationInfo(1., 10.);
  auto target = raster(shear_model_component, 5);

  std::vector<float> expected {
    0.000, 0.000, 1.000, 2.350, 1.764,
    0.000, 0.764, 2.000, 2.000, 0.764,
    0.172, 1.586, 3.000, 1.586, 0.172,
    0.764, 2.000, 2.000, 0.764, 0.000,
    1.000, 1.586, 1.000, 0.000, 0.000,
  };

  BOOST_CHECK(compareCollections(expected, target, 1e-8, 1e-2));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (TransformModelComponent_shear_integral) {
  auto linear_model_component = make_unique<LinearModelComponent>(2., 1., LinearModelComponent::Decay::BOTH);
  auto reference_raster = raster(*linear_model_component, 5);

  std::tuple<double, double, double, double> transform{1., 0.5,
                                                       0., 1.};
  TransformModelComponent transform_model_component(std::move(linear_model_component), transform);
  auto target_raster = raster(transform_model_component, 5);

  // The integral is kept through the transformation (with some tolerance!)
  double integral_scaled = std::accumulate(target_raster.begin(), target_raster.end(), 0.);
  double integral_original = std::accumulate(reference_raster.begin(), reference_raster.end(), 0.);
  BOOST_CHECK(checkIsClose(integral_original, integral_scaled, 1e-8, 1e-2));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

//-----------------------------------------------------------------------------
