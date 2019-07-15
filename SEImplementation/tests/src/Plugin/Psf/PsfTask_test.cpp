/*
 * PsfTask_test.cpp
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#include <boost/test/unit_test.hpp>
#include <numeric>
#include "SEImplementation/Plugin/Psf/PsfProperty.h"
#include "SEFramework/Source/SimpleSourceGroup.h"
#include "SEFramework/Image/MultiplyImage.h"
#include "SEImplementation/Plugin/MeasurementFrameGroupRectangle/MeasurementFrameGroupRectangle.h"
#include "SEImplementation/Plugin/Psf/PsfTask.h"

using namespace SExtractor;

void checkEqual(const std::shared_ptr<const Image<SeFloat>> &a, const  std::shared_ptr<const Image<SeFloat>> &b) {
  BOOST_REQUIRE_EQUAL(a->getWidth(), b->getWidth());
  BOOST_REQUIRE_EQUAL(a->getHeight(), b->getWidth());

  for (auto i = 0; i < a->getWidth(); ++i) {
    for (auto j = 0; j < a->getHeight(); ++j) {
      // We assume the PSF summed value will normally be around O(1)
      // so the error should remain bellow float::epsilon
      BOOST_CHECK_LE(std::abs(a->getValue(i, j) - b->getValue(i, j)), std::numeric_limits<float>::epsilon());
    }
  }
}

auto constant = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
    0. , 1., 0.,
    0.5, 1., 0.5,
    0. , 1., 0.
});
auto x = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
    0., 0., 0.,
    0., 2., 0.,
    0., 0., 0.
});
auto y = VectorImage<SeFloat>::create(3, 3, std::vector<SeFloat>{
    1., 0., 0.,
    0., 0., 0.,
    0., 0., 0.2
});

struct VariablePsfFixture {
  SimpleSourceGroup group;
  std::shared_ptr<VariablePsf> varPsf{new VariablePsf(
    1., std::vector<VariablePsf::Component>{{"X_IMAGE", 0, 5., 2.}, {"Y_IMAGE", 0, 20., 30.}},
    {1}, {constant, x, y}
  )};
  PsfTask varPsfTask{0, varPsf};
};


BOOST_AUTO_TEST_SUITE (VariablePsfProperty_test)

BOOST_FIXTURE_TEST_CASE (variable_psf_simple, VariablePsfFixture) {
  MeasurementFrameGroupRectangle measurement_rectangle(PixelCoordinate{0, 0}, PixelCoordinate{16, 100});

  double center_x = measurement_rectangle.getTopLeft().m_x + measurement_rectangle.getWidth() / 2.;
  double center_y = measurement_rectangle.getTopLeft().m_y + measurement_rectangle.getHeight() / 2.;

  // Apply offset and scale
  center_x = (center_x - 5.)/2.;
  center_y = (center_y - 20.)/30.;

  auto expected = VectorImage<SeFloat>::create(3, 3);
  double sum = 0.;
  for (int i = 0; i < expected->getHeight(); ++i) {
    for (int j = 0; j < expected->getWidth(); ++j) {
      expected->at(i, j) = constant->at(i, j) + y->at(i, j) * center_y + x->at(i, j) * center_x;
      sum += expected->at(i, j);
    }
  }
  auto expected_normalized = MultiplyImage<SeFloat>::create(expected, 1./sum);

  group.setProperty<MeasurementFrameGroupRectangle>(measurement_rectangle);

  varPsfTask.computeProperties(group);
  auto psf_prop = group.getProperty<PsfProperty>();
  checkEqual(psf_prop.getPsf(), expected_normalized);
}

BOOST_AUTO_TEST_SUITE_END ()
