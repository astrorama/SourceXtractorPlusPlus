/** Copyright © 2021 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include "SEFramework/CoordinateSystem/WCS.h"
#include "SEFramework/FITS/FitsImageSource.h"
#include <ElementsKernel/Auxiliary.h>
#include <boost/test/unit_test.hpp>

#ifdef WITH_ASDF
#include "SEFramework/ASDF/AsdfImageSource.h"
#endif

using namespace SourceXtractor;

// The wcs_header.fits file contains the headers extracted from
// EUC_MER_SCI-SWIN-NIR-J_41_20210512T213931.933876Z_00.00.fits
// That image is needed for the regression test WorldOutOfBounds_test
// Other images with other projections do not trigger the error
struct WCSFixture {
  std::string          m_fits_path;
  std::shared_ptr<WCS> m_wcs_fits;
#ifdef WITH_ASDF
  std::string          m_asdf_path;
  std::shared_ptr<WCS> m_wcs_asdf;
#endif

  WCSFixture() {
    m_fits_path = Elements::getAuxiliaryPath("wcs_header.fits").native();
    FitsImageSource fits_source(m_fits_path, 0);
    m_wcs_fits = std::make_shared<WCS>(fits_source);
#ifdef WITH_ASDF
    m_asdf_path = Elements::getAuxiliaryPath("wcs_header.asdf").native();
    AsdfImageSource asdf_source(m_asdf_path);
    m_wcs_asdf = std::make_shared<WCS>(asdf_source);
#endif
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(WCS_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ImageToWorld_test, WCSFixture) {
  std::vector<ImageCoordinate> img_coords{{0, 0}, {10, 8}, {55.5, 980.5}};
  std::vector<WorldCoordinate> world_coords{{231.36456936, 30.74785202}, {231.36564296, 30.74838044}, {231.45474466, 30.72239679}};

  for (size_t i = 0; i < img_coords.size(); ++i) {
    auto img        = img_coords[i];
    auto true_world = world_coords[i];
    auto world      = m_wcs_fits->imageToWorld(img);
    BOOST_CHECK_CLOSE(world.m_alpha, true_world.m_alpha, 1e-4);
    BOOST_CHECK_CLOSE(world.m_delta, true_world.m_delta, 1e-4);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(WorldToImage_test, WCSFixture) {
  std::vector<ImageCoordinate> img_coords{{55.36653065, 980.05611646}, {616.01593595, 1818.83360301}, {10.54811223, 945.44992869}};
  std::vector<WorldCoordinate> world_coords{{231.4547, 30.7224}, {231.55, 30.74}, {231.45, 30.72}};

  for (size_t i = 0; i < img_coords.size(); ++i) {
    auto true_img = img_coords[i];
    auto world    = world_coords[i];
    auto img      = m_wcs_fits->worldToImage(world);
    BOOST_CHECK_CLOSE(img.m_x, true_img.m_x, 1e-4);
    BOOST_CHECK_CLOSE(img.m_y, true_img.m_y, 1e-4);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ImageOutOfBounds_test, WCSFixture) {
  auto world = m_wcs_fits->imageToWorld(ImageCoordinate(-10, -5));
  BOOST_CHECK_CLOSE(world.m_alpha, 231.36376564, 1e-4);
  BOOST_CHECK_CLOSE(world.m_delta, 30.74723277, 1e-4);

  world = m_wcs_fits->imageToWorld(ImageCoordinate(2100, 2100));
  BOOST_CHECK_CLOSE(world.m_alpha, 231.62793621, 1e-4);
  BOOST_CHECK_CLOSE(world.m_delta, 30.8452462, 1e-4);
}

////-----------------------------------------------------------------------------
//
//BOOST_FIXTURE_TEST_CASE(WorldOutOfBounds_test, WCSFixture) {
//  auto img = m_wcs->worldToImage(WorldCoordinate(231.42560781394292, 30.238717631401094));
//  std::cout << img.m_x << " " << img.m_y << "\n";
//
//  BOOST_CHECK(std::isinf(img.m_x));
//}
//
//-----------------------------------------------------------------------------


#ifdef WITH_ASDF
//-----------------------------------------------------------------------------
// tests for WCS from ASDF; this mirrors the FITS tests somewhat but the
// expected results are not the same, as the existing format for embedding a
// FITS WCS in ASDF GWCS (currently used primarily by JWST and RST L3
// products) is quite a bit simplified and does not include SIP polynomials,
// etc.
//
// Expected values were checked against wcslib manually.
BOOST_FIXTURE_TEST_CASE(ASDF_ImageToWorld_test, WCSFixture) {
  std::vector<ImageCoordinate> img_coords{{0, 0}, {10, 8}, {55.5, 980.5}};
  std::vector<WorldCoordinate> world_coords{
    {269.5464167447208, 65.95659889599523},
    {269.5467894638456, 65.95672214987262},
    {269.548235426113, 65.97157594650089}
  };

  for (size_t i = 0; i < img_coords.size(); ++i) {
    auto img        = img_coords[i];
    auto true_world = world_coords[i];
    auto world      = m_wcs_asdf->imageToWorld(img);
    BOOST_CHECK_CLOSE(world.m_alpha, true_world.m_alpha, 1e-4);
    BOOST_CHECK_CLOSE(world.m_delta, true_world.m_delta, 1e-4);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ASDF_WorldToImage_test, WCSFixture) {
  std::vector<ImageCoordinate> img_coords{{0., 0.}, {10., 8.}, {55.5, 980.5}};
  std::vector<WorldCoordinate> world_coords{
    {269.5464167447208, 65.95659889599523},
    {269.5467894638456, 65.95672214987262},
    {269.548235426113, 65.97157594650089}
  };

  for (size_t i = 0; i < img_coords.size(); ++i) {
    auto true_img = img_coords[i];
    auto world    = world_coords[i];
    auto img      = m_wcs_asdf->worldToImage(world);
    if (i == 0) {
      // Value should be close to zero in the first case; BOOST_CHECK_CLOSE
      // fails on this
      BOOST_CHECK_SMALL(img.m_x, 1e-8);
      BOOST_CHECK_SMALL(img.m_y, 1e-8);
    } else {
      BOOST_CHECK_CLOSE(img.m_x, true_img.m_x, 1e-4);
      BOOST_CHECK_CLOSE(img.m_y, true_img.m_y, 1e-4);
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ASDF_ImageOutOfBounds_test, WCSFixture) {
  auto world = m_wcs_asdf->imageToWorld(ImageCoordinate(-10, -5));
  BOOST_CHECK_CLOSE(world.m_alpha, 269.54604322, 1e-4);
  BOOST_CHECK_CLOSE(world.m_delta, 65.95652145, 1e-4);

  world = m_wcs_asdf->imageToWorld(ImageCoordinate(2100, 2100));
  BOOST_CHECK_CLOSE(world.m_alpha, 269.62467272, 1e-4);
  BOOST_CHECK_CLOSE(world.m_delta, 65.98887495, 1e-4);
}
#endif /* WITH_ASDF */

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
