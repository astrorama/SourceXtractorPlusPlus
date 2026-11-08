/** Copyright © 2019-2026 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include <limits>
#include <tuple>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "SEFramework/Image/VectorImage.h"
#include "SEImplementation/Background/SE/ImageMode.h"

using namespace SourceXtractor;

namespace {

constexpr int CELL = 10;
const float INVALID = std::numeric_limits<float>::lowest();

/**
 * One cell's worth of pixels: a repeating ramp over [100.0, 101.6]
 *
 * The values have to be spread rather than constant, since a constant cell gives
 * sigma == 0, which takes a different branch in both computeBins() and
 * getBackGuess() than the one these tests are about.
 */
std::vector<float> goodCell() {
  std::vector<float> values;
  values.reserve(CELL * CELL);

  for (int i = 0; i < CELL * CELL; i++) {
    values.push_back(100.0f + static_cast<float>((i * 3) % 17) * 0.1f);
  }

  return values;
}

/// The mode and sigma of a single cell whose pixel at `index` is set to `value`
std::tuple<float, float> modeWithPixel(size_t index, float value) {
  auto values = goodCell();
  values[index] = value;

  ImageMode<float> histo(VectorImage<float>::create(CELL, CELL, values), nullptr, CELL, CELL,
                         INVALID);

  return std::make_tuple(histo.getModeImage()->getValue(0, 0),
                         histo.getSigmaImage()->getValue(0, 0));
}

}  // anonymous namespace

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ImageMode_test)

//-----------------------------------------------------------------------------

/**
 * A non-finite pixel must be discarded exactly like the invalid sentinel
 *
 * A NaN compares unequal to everything, so testing only against the sentinel let
 * it into the cell's statistics, where a single one was enough to make the mean
 * NaN, every histogram cut NaN, and the clipped selection empty.  An infinity
 * does the same by way of Inf - Inf.
 *
 * Comparing against the sentinel is the sharp form of the check: all of them
 * discard the same pixel, so all of them must agree exactly.
 */
BOOST_AUTO_TEST_CASE(non_finite_pixel_discarded_like_invalid) {
  const size_t pixel = 42;

  float expected_mode, expected_sigma;
  std::tie(expected_mode, expected_sigma) = modeWithPixel(pixel, INVALID);

  // a sanity check on the fixture itself: the good cell must produce a usable
  // background, or the comparisons below would hold vacuously
  BOOST_CHECK_NE(expected_mode, INVALID);

  for (float bad : {std::numeric_limits<float>::quiet_NaN(),
                    std::numeric_limits<float>::infinity(),
                    -std::numeric_limits<float>::infinity()}) {
    float mode, sigma;
    std::tie(mode, sigma) = modeWithPixel(pixel, bad);
    BOOST_CHECK_EQUAL(expected_mode, mode);
    BOOST_CHECK_EQUAL(expected_sigma, sigma);
  }
}

//-----------------------------------------------------------------------------

/**
 * A cell with no usable pixels is marked invalid rather than failing
 *
 * This is the path SEBackgroundLevelAnalyzer's ReplaceUndef interpolates over,
 * and it is what a mostly-empty mosaic relies on: a resampled survey skycell,
 * say, which is NaN everywhere outside its footprint (similar to a real world
 * case from Roman which tripped this).
 */
BOOST_AUTO_TEST_CASE(all_nan_cell_is_invalid) {
  std::vector<float> values(CELL * CELL, std::numeric_limits<float>::quiet_NaN());

  ImageMode<float> histo(VectorImage<float>::create(CELL, CELL, values), nullptr, CELL, CELL,
                         INVALID);

  BOOST_CHECK_EQUAL(INVALID, histo.getModeImage()->getValue(0, 0));
  BOOST_CHECK_EQUAL(INVALID, histo.getSigmaImage()->getValue(0, 0));
}

//-----------------------------------------------------------------------------

/// A mostly-NaN cell is invalid too, by the pre-existing "at least half usable" rule
BOOST_AUTO_TEST_CASE(mostly_nan_cell_is_invalid) {
  auto values = goodCell();

  for (size_t i = 0; i < 60; i++) {
    values[i] = std::numeric_limits<float>::quiet_NaN();
  }

  ImageMode<float> histo(VectorImage<float>::create(CELL, CELL, values), nullptr, CELL, CELL,
                         INVALID);

  BOOST_CHECK_EQUAL(INVALID, histo.getModeImage()->getValue(0, 0));
}

//-----------------------------------------------------------------------------

/// A cell that is NaN only in part still yields a background, from what is left
BOOST_AUTO_TEST_CASE(partially_nan_cell_is_usable) {
  auto values = goodCell();

  for (size_t i = 0; i < 30; i++) {
    values[i] = std::numeric_limits<float>::quiet_NaN();
  }

  ImageMode<float> histo(VectorImage<float>::create(CELL, CELL, values), nullptr, CELL, CELL,
                         INVALID);

  auto mode = histo.getModeImage()->getValue(0, 0);
  BOOST_CHECK_NE(mode, INVALID);
  // the surviving pixels all sit within [100, 101.6]
  BOOST_CHECK_GT(mode, 99.0f);
  BOOST_CHECK_LT(mode, 102.0f);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
