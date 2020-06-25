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

#include <numeric>
#include <boost/test/unit_test.hpp>
#include <SEImplementation/Plugin/ShapeParameters/ShapeParameters.h>
#include <SEImplementation/Plugin/KronRadius/KronRadius.h>
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Source/SimpleSource.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurve.h"
#include "SEImplementation/Plugin/GrowthCurve/GrowthCurveTask.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"

using namespace SourceXtractor;

struct GrowthCurveFixture {
  SimpleSource source;
  std::shared_ptr<VectorImage<SeFloat>> img0 = VectorImage<SeFloat>::create(5, 5, std::vector<SeFloat>{
    0, 0, 0, 0, 0,
    0, 1, 2, 1, 0,
    0, 2, 5, 2, 0,
    0, 1, 2, 1, 0,
    0, 0, 0, 0, 0,
  });
  std::shared_ptr<VectorImage<SeFloat>> img1 = img0;
  std::shared_ptr<MeasurementImageFrame> frame0, frame1;

  GrowthCurveFixture() {
    frame0 = std::make_shared<MeasurementImageFrame>(img0);
    frame1 = std::make_shared<MeasurementImageFrame>(img1);

    source.setIndexedProperty<MeasurementFrame>(0, frame0);
    source.setIndexedProperty<MeasurementFrame>(1, frame1);

    source.setIndexedProperty<MeasurementFramePixelCentroid>(0, 2, 2);
    source.setIndexedProperty<MeasurementFramePixelCentroid>(1, -5, -5);
    source.setIndexedProperty<JacobianSource>(0, 1, 0, 0, 1);
    source.setIndexedProperty<JacobianSource>(1, 1, 0, 0, 1);

    // Detection frame properties
    source.setProperty<ShapeParameters>(0.4, 0.4, 0, 0, 0, 0, 0, 0); // Only a really needed
    source.setProperty<KronRadius>(4, 0); // Kron radius must dominate 0.4*6=2.4
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (GrowthCurve_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (Simple_test, GrowthCurveFixture) {
  GrowthCurveTask task0(0, true);

  task0.computeProperties(source);
  auto& growth0 = source.getProperty<GrowthCurve>(0);

  BOOST_CHECK_EQUAL(growth0.getMax(), 4.);
  BOOST_CHECK_EQUAL(growth0.getStepSize(), 4. / 64.);

  // Last one must be equal to the sum
  SeFloat acc = std::accumulate(img0->getData().begin(), img0->getData().end(), 0.);
  BOOST_CHECK_CLOSE(acc, growth0.getCurve().back(), 1e-5);
}

//-----------------------------------------------------------------------------

// Frame0 pixels are 4x as big as the detection frame (x2 in each dimension)
// Frame1 pixels are 9x as big as the detection frame (x3 in each dimension)
// Max and step size must be set accordingly
BOOST_FIXTURE_TEST_CASE (Scaled_test, GrowthCurveFixture) {
  GrowthCurveTask task0(0, true);
  GrowthCurveTask task1(1, true);

  source.setIndexedProperty<JacobianSource>(0, 2, 0, 0, 2);
  source.setIndexedProperty<JacobianSource>(1, 3, 0, 0, 3);

  task0.computeProperties(source);
  task1.computeProperties(source);

  auto& growth0 = source.getProperty<GrowthCurve>(0);
  auto& growth1 = source.getProperty<GrowthCurve>(1);

  BOOST_CHECK_EQUAL(growth0.getMax(), 8.);
  BOOST_CHECK_EQUAL(growth0.getStepSize(), 8. / 64.);

  BOOST_CHECK_EQUAL(growth1.getMax(), 12.);
  BOOST_CHECK_EQUAL(growth1.getStepSize(), 12. / 64.);
}

//-----------------------------------------------------------------------------

// Frame0 is rotated 90, and scaled 2x
// Frame1 is flipped on the X axis, so so no effect is expected
BOOST_FIXTURE_TEST_CASE (RotatedFlip_test, GrowthCurveFixture) {
  GrowthCurveTask task0(0, true);
  GrowthCurveTask task1(1, true);

  source.setIndexedProperty<JacobianSource>(0, 0, -2, 2, 0);
  source.setIndexedProperty<JacobianSource>(1, -1, 0, 0, 1);

  task0.computeProperties(source);
  task1.computeProperties(source);

  auto& growth0 = source.getProperty<GrowthCurve>(0);
  auto& growth1 = source.getProperty<GrowthCurve>(1);

  BOOST_CHECK_EQUAL(growth0.getMax(), 8.);
  BOOST_CHECK_EQUAL(growth0.getStepSize(), 8. / 64.);

  BOOST_CHECK_EQUAL(growth1.getMax(), 4.);
  BOOST_CHECK_EQUAL(growth1.getStepSize(), 4. / 64.);
}

//-----------------------------------------------------------------------------

// Ask for the GrowthCurve property on a frame where the source is out of bounds
BOOST_FIXTURE_TEST_CASE (NotCovered_test, GrowthCurveFixture) {
  GrowthCurveTask task1(1, true);

  // Must not throw
  task1.computeProperties(source);

  auto& growth_prop = source.getProperty<GrowthCurve>(1);

  // These two must be set regardless of the source being outside
  // The transformation is the identity, and Kron Radius dominates
  BOOST_CHECK_EQUAL(growth_prop.getMax(), 4.);
  BOOST_CHECK_EQUAL(growth_prop.getStepSize(), 4. / 64.);

  // However, there is nothing to measure! So the curve must be flat
  std::vector<SeFloat> expected(64, 0.);
  auto& curve = growth_prop.getCurve();
  BOOST_CHECK_EQUAL_COLLECTIONS(expected.begin(), expected.end(), curve.begin(), curve.end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

//-----------------------------------------------------------------------------
