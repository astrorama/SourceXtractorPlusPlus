/** Copyright © 2020 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
 * @file tests/src/ASDF/AsdfImageSource_test.cpp
 * @date 10/03/2025
 * @author embray
 */

#include <boost/test/unit_test.hpp>

#include <ElementsKernel/Auxiliary.h>

#include "SEFramework/ASDF/AsdfImageSource.h"
#include "SEFramework/Image/ImageAccessor.h"

using namespace SourceXtractor;

struct AsdfImageSourceFixture {
  std::string mhdu_path, primary_path, wcs_header;

  AsdfImageSourceFixture() {
    mhdu_path = Elements::getAuxiliaryPath("multiple_hdu.asdf").native();
    primary_path = Elements::getAuxiliaryPath("with_primary.asdf").native();
    wcs_header = Elements::getAuxiliaryPath("wcs_header.asdf").native();
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(AsdfImageSource_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(primary_test, AsdfImageSourceFixture) {
  auto img_src = std::make_shared<AsdfImageSource>(primary_path, 0, ImageTile::FloatImage);
  BOOST_CHECK_EQUAL(img_src->getHeight(), 1);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 1);
  BOOST_CHECK_CLOSE(img_src->getImageTile(0, 0, 1, 1)->getValue<SeFloat>(0, 0), 1024.44f, 1e-8);
}

//-----------------------------------------------------------------------------

// NOTE: This claims to test a compressed image? At least based on the name
// But the FITS file this test was based on does not contain a compressed image either...
BOOST_FIXTURE_TEST_CASE(compressed_image_test, AsdfImageSourceFixture) {
  auto img_src = std::make_shared<AsdfImageSource>(mhdu_path, 0, ImageTile::FloatImage);
  BOOST_CHECK_EQUAL(img_src->getHeight(), 1);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 1);
  BOOST_CHECK_CLOSE(img_src->getImageTile(0, 0, 1, 1)->getValue<SeFloat>(0, 0), 256.2f, 1e-8);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(second_image_test, AsdfImageSourceFixture) {
  auto img_src = std::make_shared<AsdfImageSource>(mhdu_path, 1, ImageTile::FloatImage);
  BOOST_CHECK_EQUAL(img_src->getHeight(), 1);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 1);
  BOOST_CHECK_CLOSE(img_src->getImageTile(0, 0, 1, 1)->getValue<SeFloat>(0, 0), 1024.44f, 1e-8);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(second_image_by_name_test, AsdfImageSourceFixture) {
  auto img_src = std::make_shared<AsdfImageSource>(mhdu_path, "IMAGE2", ImageTile::FloatImage);
  BOOST_CHECK_EQUAL(img_src->getHeight(), 1);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 1);
  BOOST_CHECK_CLOSE(img_src->getImageTile(0, 0, 1, 1)->getValue<SeFloat>(0, 0), 1024.44f, 1e-8);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(ndarray_is_table_test, AsdfImageSourceFixture) {
  BOOST_CHECK_THROW(std::make_shared<AsdfImageSource>(mhdu_path, 3), Elements::Exception);
  BOOST_CHECK_THROW(std::make_shared<AsdfImageSource>(mhdu_path, "TABLE"), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(bad_ndarray_test, AsdfImageSourceFixture) {
  BOOST_CHECK_THROW(std::make_shared<AsdfImageSource>(mhdu_path, 5), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(empty_ndarray_test, AsdfImageSourceFixture) {
  BOOST_CHECK_THROW(std::make_shared<AsdfImageSource>(mhdu_path, 2), Elements::Exception);
  BOOST_CHECK_THROW(std::make_shared<AsdfImageSource>(mhdu_path, "PRIMARY"), Elements::Exception);
}


BOOST_FIXTURE_TEST_CASE(get_fitswcs_auto, AsdfImageSourceFixture) {
  auto img_src = std::make_shared<AsdfImageSource>(wcs_header, "data", ImageTile::FloatImage);
  auto fits_wcs = img_src->getFitsWCS();
  BOOST_CHECK(fits_wcs != nullptr);
  // The full WCS is checked in AsdfFile_test, but here make sure we just grab the expected
  // one.  There are two WCS in the file each with different expected ctypes.  The first one,
  // "wcs", should be TAN, the other is AIR
  auto ctype = fits_wcs->ctype();
  std::array<std::string_view, 2> expected_ctype{{"RA---TAN", "DEC--TAN"}};
  BOOST_CHECK_EQUAL_COLLECTIONS(
    ctype.begin(), ctype.end(),
    expected_ctype.begin(), expected_ctype.end()
  );
}


BOOST_FIXTURE_TEST_CASE(get_fitswcs_simple_path, AsdfImageSourceFixture) {
  auto img_src = std::make_shared<AsdfImageSource>(wcs_header, "data", ImageTile::FloatImage);
  auto fits_wcs = img_src->getFitsWCS("wcs2");
  BOOST_CHECK(fits_wcs != nullptr);
  // The full WCS is checked in AsdfFile_test, but here make sure we just grab the expected
  // one.  There are two WCS in the file each with different expected ctypes.  The first one,
  // "wcs", should be TAN, the other is AIR
  auto ctype = fits_wcs->ctype();
  std::array<std::string_view, 2> expected_ctype{{"RA---AIR", "DEC--AIR"}};
  BOOST_CHECK_EQUAL_COLLECTIONS(
    ctype.begin(), ctype.end(),
    expected_ctype.begin(), expected_ctype.end()
  );
}


BOOST_FIXTURE_TEST_CASE(get_fitswcs_mapped_path, AsdfImageSourceFixture) {
  auto img_src = std::make_shared<AsdfImageSource>(wcs_header, "data", ImageTile::FloatImage);
  auto fits_wcs = img_src->getFitsWCS("data:wcs2");
  BOOST_CHECK(fits_wcs != nullptr);
  // The full WCS is checked in AsdfFile_test, but here make sure we just grab the expected
  // one.  There are two WCS in the file each with different expected ctypes.  The first one,
  // "wcs", should be TAN, the other is AIR
  auto ctype = fits_wcs->ctype();
  std::array<std::string_view, 2> expected_ctype{{"RA---AIR", "DEC--AIR"}};
  BOOST_CHECK_EQUAL_COLLECTIONS(
    ctype.begin(), ctype.end(),
    expected_ctype.begin(), expected_ctype.end()
  );

  BOOST_CHECK_THROW(img_src->getFitsWCS("data:does-not-exist"), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

