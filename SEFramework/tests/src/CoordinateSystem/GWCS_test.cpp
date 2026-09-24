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
 * libasdf-gwcs evaluates the forward transform only; the reverse has to fail
 * loudly rather than return something plausible but wrong.
 */
BOOST_FIXTURE_TEST_CASE(WorldToImageThrows_test, GWCSFixture) {
  BOOST_CHECK_THROW(m_gwcs->worldToImage(WorldCoordinate(269.5464167447208, 65.95659889599523)),
                    InvalidCoordinatesException);
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

BOOST_FIXTURE_TEST_CASE(BadPathThrows_test, GWCSFixture) {
  AsdfImageSource asdf_source(m_asdf_path);
  BOOST_CHECK_THROW(asdf_source.getCoordinateSystem("does-not-exist"), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
