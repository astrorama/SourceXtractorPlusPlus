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
 * ScaledModelComponent_test.cpp
 *
 *  Created on: July 03, 2019
 *      Author: Alejandro Alvarez Ayllon
 */
#include <boost/test/unit_test.hpp>
#include <numeric>
#include "AlexandriaKernel/memory_tools.h"
#include "ModelFitting/Models/OnlySmooth.h"
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Models/ScaledModelComponent.h"
#include "SEUtils/TestUtils.h"
#include "TestHelper.h"

using namespace ModelFitting;
using namespace SourceXtractor;
using Euclid::make_unique;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ScaledModelComponent_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (ScaledModelComponent_noop_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(3., 1., LinearModelComponent::Decay::X_AXIS);

  auto scale_x = std::make_shared<ManualParameter>(1);
  auto scale_y = std::make_shared<ManualParameter>(1);
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

  auto scale_x = std::make_shared<ManualParameter>(2.);
  auto scale_y = std::make_shared<ManualParameter>(2.);
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

  auto scale_x = std::make_shared<ManualParameter>(3.);
  auto scale_y = std::make_shared<ManualParameter>(3.);
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
  auto reference = raster(*linear_model_component, 5);

  auto scale_x = std::make_shared<ManualParameter>(2.);
  auto scale_y = std::make_shared<ManualParameter>(1.);
  ScaledModelComponent scaled_model_component(std::move(linear_model_component), scale_x, scale_y);

  auto target = raster(scaled_model_component, 7);

  std::vector<float> expected {
    0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000,
    0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000,
    0.197, 0.586, 0.882, 1.000, 0.882, 0.586, 0.197,
    0.500, 1.000, 1.500, 2.000, 1.500, 1.000, 0.500,
    0.197, 0.586, 0.882, 1.000, 0.882, 0.586, 0.197,
    0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000,
    0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000,
  };

  BOOST_CHECK(compareCollections(expected, target, 1e-8, 1e-2));

  double reference_integrated = std::accumulate(reference.begin(), reference.end(), 0.);
  double target_integrated = std::accumulate(target.begin(), target.end(), 0.);

  BOOST_CHECK(checkIsClose(reference_integrated*2, target_integrated, 1e-8, 1e-2));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (ScaledModelComponent_twice_2_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(2., 1., LinearModelComponent::Decay::BOTH);
  auto reference = raster(*linear_model_component, 5);

  std::vector<float> reference_expected {
    0.000, 0.000, 0.000, 0.000, 0.000,
    0.000, 0.586, 1.000, 0.586, 0.000,
    0.000, 1.000, 2.000, 1.000, 0.000,
    0.000, 0.586, 1.000, 0.586, 0.000,
    0.000, 0.000, 0.000, 0.000, 0.000,
  };

  BOOST_CHECK(compareCollections(reference_expected, reference, 1e-8, 1e-2));

  auto scale_x = std::make_shared<ManualParameter>(2.);
  auto scale_y = std::make_shared<ManualParameter>(2.);
  ScaledModelComponent scaled_model_component(std::move(linear_model_component), scale_x, scale_y);
  scaled_model_component.updateRasterizationInfo(1., 10.);

  auto target = raster(scaled_model_component, 5);

  std::vector<float> expected {
    0.586, 0.882, 1.000, 0.882, 0.586,
    0.882, 1.293, 1.500, 1.293, 0.882,
    1.000, 1.500, 2.000, 1.500, 1.000,
    0.882, 1.293, 1.500, 1.293, 0.882,
    0.586, 0.882, 1.000, 0.882, 0.586,
  };

  BOOST_CHECK(compareCollections(expected, target, 1e-8, 1e-2));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE (ScaledModelComponent_twice_sharp_test) {
  auto linear_model_component = make_unique<LinearModelComponent>(2., 1., LinearModelComponent::Decay::BOTH, 3);
  auto reference = raster(*linear_model_component, 5);

  std::vector<float> reference_expected {
    0.000, 0.000, 0.000, 0.000, 0.000,
    0.000, 0.586, 1.000, 0.586, 0.000,
    0.000, 1.000, 2.000, 1.000, 0.000,
    0.000, 0.586, 1.000, 0.586, 0.000,
    0.000, 0.000, 0.000, 0.000, 0.000,
  };

  BOOST_CHECK(compareCollections(reference_expected, reference, 1e-8, 1e-2));

  auto scale_x = std::make_shared<ManualParameter>(2.);
  auto scale_y = std::make_shared<ManualParameter>(2.);
  ScaledModelComponent scaled_model_component(std::move(linear_model_component), scale_x, scale_y);
  scaled_model_component.updateRasterizationInfo(1., 10.);

  auto target = raster(scaled_model_component, 5);

  std::vector<float> expected {
    0.586, 0.882, 1.000, 0.882, 0.586,
    0.882, 1.293, 1.500, 1.293, 0.882,
    1.000, 1.500, 2.000, 1.500, 1.000,
    0.882, 1.293, 1.500, 1.293, 0.882,
    0.586, 0.882, 1.000, 0.882, 0.586,
  };

  BOOST_CHECK(compareCollections(expected, target, 1e-8, 1e-2));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

//-----------------------------------------------------------------------------
