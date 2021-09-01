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

using namespace SourceXtractor;

// The wcs_header.fits file contains the headers extracted from
// EUC_MER_SCI-SWIN-NIR-J_41_20210512T213931.933876Z_00.00.fits
// That image is needed for the regression test WordOutOfBounds_test
// Other images with other projections do not trigger the error
struct WCSFixture {
  std::string          m_fits_path;
  std::shared_ptr<WCS> m_wcs;

  WCSFixture() {
    m_fits_path = Elements::getAuxiliaryPath("wcs_header.fits").native();
    FitsImageSource fits_source(m_fits_path, 0);
    m_wcs = std::make_shared<WCS>(fits_source);
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(WCS_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ImageToWord_test, WCSFixture) {
  std::vector<ImageCoordinate> img_coords{{0, 0}, {10, 8}, {55.5, 980.5}};
  std::vector<WorldCoordinate> world_coords{{231.36456936, 30.74785202}, {231.36564296, 30.74838044}, {231.45474466, 30.72239679}};

  for (size_t i = 0; i < img_coords.size(); ++i) {
    auto img        = img_coords[i];
    auto true_world = world_coords[i];
    auto world      = m_wcs->imageToWorld(img);
    BOOST_CHECK_CLOSE(world.m_alpha, true_world.m_alpha, 1e-4);
    BOOST_CHECK_CLOSE(world.m_delta, true_world.m_delta, 1e-4);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(WordToImage_test, WCSFixture) {
  std::vector<ImageCoordinate> img_coords{{55.36653065, 980.05611646}, {616.01593595, 1818.83360301}, {10.54811223, 945.44992869}};
  std::vector<WorldCoordinate> world_coords{{231.4547, 30.7224}, {231.55, 30.74}, {231.45, 30.72}};

  for (size_t i = 0; i < img_coords.size(); ++i) {
    auto true_img = img_coords[i];
    auto world    = world_coords[i];
    auto img      = m_wcs->worldToImage(world);
    BOOST_CHECK_CLOSE(img.m_x, true_img.m_x, 1e-4);
    BOOST_CHECK_CLOSE(img.m_y, true_img.m_y, 1e-4);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ImageOutOfBounds_test, WCSFixture) {
  auto world = m_wcs->imageToWorld(ImageCoordinate(-10, -5));
  BOOST_CHECK_CLOSE(world.m_alpha, 231.36376564, 1e-4);
  BOOST_CHECK_CLOSE(world.m_delta, 30.74723277, 1e-4);

  world = m_wcs->imageToWorld(ImageCoordinate(2100, 2100));
  BOOST_CHECK_CLOSE(world.m_alpha, 231.62793621, 1e-4);
  BOOST_CHECK_CLOSE(world.m_delta, 30.8452462, 1e-4);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(WordOutOfBounds_test, WCSFixture) {
  BOOST_CHECK_THROW(m_wcs->worldToImage(WorldCoordinate(231.42560781394292, 30.238717631401094)), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
