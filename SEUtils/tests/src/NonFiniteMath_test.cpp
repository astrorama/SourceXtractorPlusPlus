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
/**
 * @file tests/src/NonFiniteMath_test.cpp
 *
 * Detection images legitimately contain NaN: for floating-point FITS the standard
 * has no other way to mark an undefined pixel, since BLANK is integer-only, and
 * mosaics blank their unexposed area that way.  Under -ffinite-math-only a NaN is
 * undefined behaviour rather than merely unlikely, so every isnan/isinf test is
 * compiled out and the result of any comparison against one becomes arbitrary --
 * differing between compilers and between architectures.
 *
 * These checks fail loudly at build-verification time rather than leaving it to be
 * discovered as a crash or a hang on real data.
 */

#include <boost/test/unit_test.hpp>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

namespace {

/// The bit pattern of a 32-bit quiet NaN, read through volatile so that no
/// amount of constant folding can decide the answers below at compile time
volatile uint32_t g_quiet_nan_bits = 0x7FC00000u;

float runtimeNaN() {
  uint32_t bits = g_quiet_nan_bits;
  float value;
  std::memcpy(&value, &bits, sizeof(value));
  return value;
}

}  // anonymous namespace

BOOST_AUTO_TEST_SUITE(NonFiniteMath_test)

//-----------------------------------------------------------------------------

/// Both GCC and Clang define this to 1 exactly when -ffinite-math-only is in effect,
/// so unlike the behavioural checks below it cannot accidentally pass
BOOST_AUTO_TEST_CASE(finite_math_only_is_not_enabled_test) {
#ifdef __FINITE_MATH_ONLY__
  BOOST_CHECK_MESSAGE(__FINITE_MATH_ONLY__ == 0,
                      "built with -ffinite-math-only (implied by -ffast-math): NaN/Inf behavior "
                      "is ill defined. Configure with -DFINITE_MATH_ONLY=OFF, "
                      "see cmake/LocalBuildFlags.cmake");
#endif
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(nan_is_classified_test) {
  float nan_value = runtimeNaN();

  BOOST_CHECK(std::isnan(nan_value));
  BOOST_CHECK(!std::isfinite(nan_value));
  // The canonical idiom, and the one an optimiser is most likely to fold away
  BOOST_CHECK(nan_value != nan_value);
}

//-----------------------------------------------------------------------------

/// Two comparisons this code base actually relies on: the sentinel test in
/// ImageMode::processCell and the above-threshold test in Lutz::labelImage
BOOST_AUTO_TEST_CASE(nan_comparisons_are_ieee_test) {
  float nan_value = runtimeNaN();
  const float sentinel = std::numeric_limits<float>::lowest();

  BOOST_CHECK(nan_value != sentinel);
  BOOST_CHECK(!(nan_value == sentinel));

  BOOST_CHECK(!(nan_value > 0.0f));
  BOOST_CHECK(!(nan_value < 0.0f));
  BOOST_CHECK(!(nan_value >= 0.0f));
  BOOST_CHECK(!(nan_value <= 0.0f));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
