/**
 * Copyright © 2019-2022 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include <atomic>
#include <thread>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <ElementsKernel/Auxiliary.h>

#include "SEFramework/ASDF/AsdfImageSource.h"
#include "SEFramework/CoordinateSystem/GWCS.h"

using namespace SourceXtractor;

/**
 * wcs_header.asdf holds two GWCS objects with deliberately different
 * projections, so that selecting between them by path is observable:
 * /wcs1 is RA---TAN and /wcs2 is RA---AIR.
 */
struct GWCSFixture {
  std::string                       m_asdf_path;
  std::shared_ptr<CoordinateSystem> m_gwcs;

  GWCSFixture() {
    m_asdf_path = Elements::getAuxiliaryPath("wcs_header.asdf").native();
    AsdfImageSource asdf_source(m_asdf_path);
    m_gwcs = asdf_source.getCoordinateSystem();
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(GWCS_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(available_test, GWCSFixture) {
  BOOST_CHECK(m_gwcs != nullptr);
  BOOST_CHECK(std::dynamic_pointer_cast<GWCS>(m_gwcs) != nullptr);
}

//-----------------------------------------------------------------------------

/**
 * The expected values are the ones the old fitswcs_imaging-to-wcslib path
 * produced, and were checked against wcslib by hand.  /wcs1 is a plain
 * fitswcs_imaging transform, so evaluating it through libasdf-gwcs must agree:
 * this is a cross-check of the AST backend against wcslib.
 */
BOOST_FIXTURE_TEST_CASE(ImageToWorld_test, GWCSFixture) {
  std::vector<ImageCoordinate> img_coords{{0, 0}, {10, 8}, {55.5, 980.5}};
  std::vector<WorldCoordinate> world_coords{
    {269.5464167447208, 65.95659889599523},
    {269.5467894638456, 65.95672214987262},
    {269.548235426113, 65.97157594650089}
  };

  for (size_t idx = 0; idx < img_coords.size(); idx++) {
    auto world = m_gwcs->imageToWorld(img_coords[idx]);
    BOOST_CHECK_CLOSE(world.m_alpha, world_coords[idx].m_alpha, 1e-9);
    BOOST_CHECK_CLOSE(world.m_delta, world_coords[idx].m_delta, 1e-9);
  }
}

//-----------------------------------------------------------------------------

/**
 * AST returns longitudes in (-180, 180], so without normalisation these would
 * come back around -90 rather than 269.
 */
BOOST_FIXTURE_TEST_CASE(RightAscensionIsNormalized_test, GWCSFixture) {
  for (auto img : {ImageCoordinate(0, 0), ImageCoordinate(1000, 1000),
                   ImageCoordinate(-500, -500)}) {
    auto world = m_gwcs->imageToWorld(img);
    BOOST_CHECK_GE(world.m_alpha, 0.);
    BOOST_CHECK_LT(world.m_alpha, 360.);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ImageOutOfBounds_test, GWCSFixture) {
  auto world = m_gwcs->imageToWorld(ImageCoordinate(-10, -5));
  BOOST_CHECK_CLOSE(world.m_alpha, 269.54604322, 1e-4);
  BOOST_CHECK_CLOSE(world.m_delta, 65.95652145, 1e-4);

  world = m_gwcs->imageToWorld(ImageCoordinate(2100, 2100));
  BOOST_CHECK_CLOSE(world.m_alpha, 269.62467272, 1e-4);
  BOOST_CHECK_CLOSE(world.m_delta, 65.98887495, 1e-4);
}

//-----------------------------------------------------------------------------

/**
 * The forward direction is validated against wcslib and astropy above, so a
 * clean round trip is what establishes the inverse: pixel -> sky -> pixel must
 * land back where it started.
 */
BOOST_FIXTURE_TEST_CASE(RoundTrip_test, GWCSFixture) {
  std::vector<ImageCoordinate> img_coords{
    {0, 0}, {10, 8}, {55.5, 980.5}, {255, 255}, {500, 100}
  };

  for (const auto& img : img_coords) {
    auto world = m_gwcs->imageToWorld(img);
    auto back = m_gwcs->worldToImage(world);

    BOOST_CHECK_SMALL(back.m_x - img.m_x, 1e-6);
    BOOST_CHECK_SMALL(back.m_y - img.m_y, 1e-6);
  }
}

//-----------------------------------------------------------------------------

/**
 * imageToWorld normalizes longitude to [0, 360) while AST works in
 * (-180, 180]; the inverse has to accept what the forward direction produces.
 */
BOOST_FIXTURE_TEST_CASE(RoundTripAcceptsNormalizedLongitude_test, GWCSFixture) {
  auto world = m_gwcs->imageToWorld(ImageCoordinate(10, 8));
  BOOST_REQUIRE_GE(world.m_alpha, 0.);
  BOOST_REQUIRE_LT(world.m_alpha, 360.);

  // Same position expressed in (-180, 180] must give the same pixel
  WorldCoordinate shifted(world.m_alpha - 360.0, world.m_delta);

  auto from_normalized = m_gwcs->worldToImage(world);
  auto from_shifted = m_gwcs->worldToImage(shifted);

  BOOST_CHECK_SMALL(from_normalized.m_x - 10.0, 1e-6);
  BOOST_CHECK_SMALL(from_normalized.m_y - 8.0, 1e-6);
  BOOST_CHECK_SMALL(from_shifted.m_x - from_normalized.m_x, 1e-9);
  BOOST_CHECK_SMALL(from_shifted.m_y - from_normalized.m_y, 1e-9);
}

//-----------------------------------------------------------------------------

/**
 * A sky position outside the projection's valid region has no image
 * coordinates.  AST reports that as AST__BAD with no error code, so without an
 * explicit check worldToImage would hand back -DBL_MAX.  Callers such as
 * MeasurementFrameRectangleTask catch InvalidCoordinatesException and read it
 * as "this source is not on that frame".
 */
BOOST_FIXTURE_TEST_CASE(WorldToImageOutsideProjectionThrows_test, GWCSFixture) {
  // The fixture's WCS is centred near (269.5, +66); the opposite hemisphere is
  // not representable in a TAN projection
  for (auto world : {WorldCoordinate(269.5, -65.9), WorldCoordinate(0.0, 0.0),
                     WorldCoordinate(89.5, -89.0)}) {
    BOOST_CHECK_THROW(m_gwcs->worldToImage(world), InvalidCoordinatesException);
  }
}

//-----------------------------------------------------------------------------

/**
 * worldToImage runs on the measurement threads just as imageToWorld does, and
 * builds its inverse context per call, so it needs the same guarantee.
 */
BOOST_FIXTURE_TEST_CASE(ConcurrentWorldToImage_test, GWCSFixture) {
  std::vector<WorldCoordinate> coords;
  for (int idx = 0; idx < 32; idx++) {
    coords.push_back(m_gwcs->imageToWorld(ImageCoordinate(idx * 11.0, idx * 5.0)));
  }

  std::vector<ImageCoordinate> expected;
  for (const auto& world : coords) {
    expected.push_back(m_gwcs->worldToImage(world));
  }

  constexpr int n_threads = 8;
  std::vector<std::thread> threads;
  std::atomic<int> mismatches{0};
  std::atomic<int> failures{0};

  for (int t = 0; t < n_threads; t++) {
    threads.emplace_back([&]() {
      for (int rep = 0; rep < 20; rep++) {
        for (size_t idx = 0; idx < coords.size(); idx++) {
          try {
            auto img = m_gwcs->worldToImage(coords[idx]);
            if (img.m_x != expected[idx].m_x || img.m_y != expected[idx].m_y) {
              ++mismatches;
            }
          } catch (...) {
            ++failures;
          }
        }
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  BOOST_CHECK_EQUAL(failures.load(), 0);
  BOOST_CHECK_EQUAL(mismatches.load(), 0);
}

//-----------------------------------------------------------------------------

/**
 * Selecting the WCS by path picks a different one, with a different projection
 */
BOOST_FIXTURE_TEST_CASE(SelectByPath_test, GWCSFixture) {
  AsdfImageSource asdf_source(m_asdf_path);

  auto wcs1 = asdf_source.getCoordinateSystem("wcs1");
  auto wcs2 = asdf_source.getCoordinateSystem("wcs2");

  BOOST_REQUIRE(wcs1 != nullptr);
  BOOST_REQUIRE(wcs2 != nullptr);

  auto world1 = wcs1->imageToWorld(ImageCoordinate(10, 8));
  auto world2 = wcs2->imageToWorld(ImageCoordinate(10, 8));

  // The default is the first WCS found, which is wcs1
  auto world_default = m_gwcs->imageToWorld(ImageCoordinate(10, 8));
  BOOST_CHECK_CLOSE(world1.m_alpha, world_default.m_alpha, 1e-8);
  BOOST_CHECK_CLOSE(world1.m_delta, world_default.m_delta, 1e-8);

  // wcs2 uses a different projection, so it must not agree with wcs1
  BOOST_CHECK(std::abs(world1.m_alpha - world2.m_alpha) > 1e-6 ||
              std::abs(world1.m_delta - world2.m_delta) > 1e-6);
}

//-----------------------------------------------------------------------------

/**
 * A single CoordinateSystem is shared by every source in a frame and evaluated
 * from the measurement thread pool, so concurrent imageToWorld must give the
 * same answers as a serial one.  This previously blocked due to AST's
 * threading model, but libasdf-gwcs now provides asdf_gwcs_eval_copy, which
 * is a thread-safe way to make a per-thread copy of the evaluation context
 * for concurrent use--similarly to how the WCS class takes a per-thread copy
 * of the wcsprm struct.
 */
BOOST_FIXTURE_TEST_CASE(ConcurrentImageToWorld_test, GWCSFixture) {
  std::vector<ImageCoordinate> coords;
  for (int idx = 0; idx < 64; idx++) {
    coords.emplace_back(idx * 7.5, idx * 3.25);
  }

  // Reference, computed on this thread
  std::vector<WorldCoordinate> expected;
  for (const auto& coord : coords) {
    expected.push_back(m_gwcs->imageToWorld(coord));
  }

  constexpr int n_threads = 8;
  std::vector<std::thread> threads;
  std::atomic<int> mismatches{0};
  std::atomic<int> failures{0};

  for (int t = 0; t < n_threads; t++) {
    threads.emplace_back([&]() {
      for (int rep = 0; rep < 50; rep++) {
        for (size_t idx = 0; idx < coords.size(); idx++) {
          try {
            auto world = m_gwcs->imageToWorld(coords[idx]);
            if (world.m_alpha != expected[idx].m_alpha ||
                world.m_delta != expected[idx].m_delta) {
              ++mismatches;
            }
          } catch (...) {
            ++failures;
          }
        }
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  BOOST_CHECK_EQUAL(failures.load(), 0);
  BOOST_CHECK_EQUAL(mismatches.load(), 0);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(BadPathThrows_test, GWCSFixture) {
  AsdfImageSource asdf_source(m_asdf_path);
  BOOST_CHECK_THROW(asdf_source.getCoordinateSystem("does-not-exist"), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
