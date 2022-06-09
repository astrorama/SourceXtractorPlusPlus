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
 * JacobianTask_test.cpp
 *
 *  Created on: Jul 11, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Source/SimpleSourceGroup.h"
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h"
#include "SEImplementation/Plugin/DetectionFrameCoordinates/DetectionFrameCoordinates.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFrameCoordinates/MeasurementFrameCoordinates.h"
#include "SEImplementation/Plugin/Jacobian/JacobianTask.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "SEUtils/TestUtils.h"
#include <AlexandriaKernel/memory_tools.h>

using namespace SourceXtractor;

/**
 * No Transformation is done: 1 to 1
 */
class NoopCoordinateSystem : public CoordinateSystem {

public:
  WorldCoordinate imageToWorld(ImageCoordinate image_coordinate) const override {
    return {image_coordinate.m_x, image_coordinate.m_y};
  }

  ImageCoordinate worldToImage(WorldCoordinate world_coordinate) const override {
    return {world_coordinate.m_alpha, world_coordinate.m_delta};
  }
};

/**
 * Transformation is done: scaling
 */
class ScaleCoordinateSystem : public CoordinateSystem {
private:
  float m_scale;

public:

  ScaleCoordinateSystem(float s) : m_scale(s) {}

  WorldCoordinate imageToWorld(ImageCoordinate image_coordinate) const override {
    return {image_coordinate.m_x / m_scale, image_coordinate.m_y / m_scale};
  }

  ImageCoordinate worldToImage(WorldCoordinate world_coordinate) const override {
    return {world_coordinate.m_alpha * m_scale, world_coordinate.m_delta * m_scale};
  }
};

/**
 * Transformation is done: scaling
 */
class ShearCoordinates : public CoordinateSystem {
public:

  WorldCoordinate imageToWorld(ImageCoordinate image_coordinate) const override {
    return {image_coordinate.m_x - image_coordinate.m_y, image_coordinate.m_y};
  }

  ImageCoordinate worldToImage(WorldCoordinate world_coordinate) const override {
    return {world_coordinate.m_alpha + world_coordinate.m_delta , world_coordinate.m_delta};
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (JacobianGroupTask_test)

//-----------------------------------------------------------------------------

/**
 * There is no transformation whatsoever: one to one correspondence, so the
 * Jacobian is the identity
 */
BOOST_AUTO_TEST_CASE (JacobianIdentity_test) {
  auto jacobian_task = JacobianGroupTask(0);

  auto source = Euclid::make_unique<SimpleSource>();
  auto measurement_coord_system = std::make_shared<NoopCoordinateSystem>();
  auto detection_coord_system = std::make_shared<NoopCoordinateSystem>();
  source->setProperty<MeasurementFrameCoordinates>(measurement_coord_system);
  source->setProperty<DetectionFrameCoordinates>(detection_coord_system);

  auto world_center = detection_coord_system->imageToWorld({150, 150});
  auto measurement_center = measurement_coord_system->worldToImage(world_center);
  BOOST_CHECK_EQUAL(150, measurement_center.m_x);
  BOOST_CHECK_EQUAL(150, measurement_center.m_y);

  SimpleSourceGroup group;
  group.addSource(std::move(source));

  group.setProperty<DetectionFrameGroupStamp>(
    ConstantImage<SeFloat>::create(100, 100, 0), nullptr, PixelCoordinate{100, 100}, nullptr
  );

  jacobian_task.computeProperties(group);
  auto jacobian = group.getProperty<JacobianGroup>().asTuple();

  BOOST_CHECK(checkIsClose(std::get<0>(jacobian), 1.));
  BOOST_CHECK(checkIsClose(std::get<1>(jacobian), 0.));
  BOOST_CHECK(checkIsClose(std::get<2>(jacobian), 0.));
  BOOST_CHECK(checkIsClose(std::get<3>(jacobian), 1.));
}

//-----------------------------------------------------------------------------

/**
 * Every pixel in the detection image corresponds to 4 on the measurement image (square of 2x2)
 */
BOOST_AUTO_TEST_CASE (JacobianScale_test) {
  auto jacobian_task = JacobianGroupTask(0);

  auto source = Euclid::make_unique<SimpleSource>();
  auto measurement_coord_system = std::make_shared<ScaleCoordinateSystem>(2);
  auto detection_coord_system = std::make_shared<NoopCoordinateSystem>();
  source->setProperty<MeasurementFrameCoordinates>(measurement_coord_system);
  source->setProperty<DetectionFrameCoordinates>(detection_coord_system);

  auto world_center = detection_coord_system->imageToWorld({150, 150});
  auto measurement_center = measurement_coord_system->worldToImage(world_center);
  BOOST_CHECK_EQUAL(300, measurement_center.m_x);
  BOOST_CHECK_EQUAL(300, measurement_center.m_y);

  SimpleSourceGroup group;
  group.addSource(std::move(source));

  group.setProperty<DetectionFrameGroupStamp>(
    ConstantImage<SeFloat>::create(100, 100, 0), nullptr, PixelCoordinate{100, 100}, nullptr
  );

  jacobian_task.computeProperties(group);
  auto jacobian = group.getProperty<JacobianGroup>().asTuple();

  BOOST_CHECK(checkIsClose(std::get<0>(jacobian), 2.));
  BOOST_CHECK(checkIsClose(std::get<1>(jacobian), 0.));
  BOOST_CHECK(checkIsClose(std::get<2>(jacobian), 0.));
  BOOST_CHECK(checkIsClose(std::get<3>(jacobian), 2.));
}

//-----------------------------------------------------------------------------

/**
 * Measurement frame is deformed
 */
BOOST_AUTO_TEST_CASE (JacobianShear_test) {
  auto jacobian_task = JacobianGroupTask(0);

  auto source = Euclid::make_unique<SimpleSource>();
  auto measurement_coord_system = std::make_shared<ShearCoordinates>();
  auto detection_coord_system = std::make_shared<NoopCoordinateSystem>();
  source->setProperty<MeasurementFrameCoordinates>(measurement_coord_system);
  source->setProperty<DetectionFrameCoordinates>(detection_coord_system);

  auto world_center = detection_coord_system->imageToWorld({150, 150});
  auto measurement_center = measurement_coord_system->worldToImage(world_center);
  BOOST_CHECK_EQUAL(300, measurement_center.m_x);
  BOOST_CHECK_EQUAL(150, measurement_center.m_y);

  SimpleSourceGroup group;
  group.addSource(std::move(source));

  group.setProperty<DetectionFrameGroupStamp>(
    ConstantImage<SeFloat>::create(100, 100, 0), nullptr, PixelCoordinate{100, 100}, nullptr
  );

  jacobian_task.computeProperties(group);
  auto jacobian = group.getProperty<JacobianGroup>().asTuple();

  BOOST_CHECK(checkIsClose(std::get<0>(jacobian), 1.));
  BOOST_CHECK(checkIsClose(std::get<1>(jacobian), 0.));
  BOOST_CHECK(checkIsClose(std::get<2>(jacobian), 1.));
  BOOST_CHECK(checkIsClose(std::get<3>(jacobian), 1.));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

//-----------------------------------------------------------------------------
