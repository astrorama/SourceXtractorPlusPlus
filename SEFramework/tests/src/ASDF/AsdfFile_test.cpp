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
 * @file tests/src/AsdfFile_test.cpp
 * @date 08/14/25
 * @author embray
 */

#include <boost/test/unit_test.hpp>

#include <ElementsKernel/Auxiliary.h>
#include <ElementsKernel/Exception.h>

#include "SEFramework/ASDF/AsdfFile.h"

using namespace SourceXtractor;


struct AsdfFileFixture {
  std::string primary_path;
  std::string wcs_path;

  AsdfFileFixture() {
    primary_path = Elements::getAuxiliaryPath("with_primary.asdf").native();
    wcs_path = Elements::getAuxiliaryPath("wcs_header.asdf").native();
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (AsdfFile_test)


//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( open_file, AsdfFileFixture ) {
  BOOST_CHECK_NO_THROW({
    AsdfFile asdf_file(primary_path);
  });
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( missing_file ) {
  BOOST_CHECK_THROW({
    AsdfFile asdf_file("/does/not/exist");
  }, Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( get_ndarray_by_index, AsdfFileFixture ) {
  AsdfFile asdf_file(primary_path);
  auto ndarray = asdf_file.getNdarray(0);
  BOOST_CHECK_EQUAL(ndarray->ndim(), 2);
  auto shape = ndarray->shape();
  std::vector<uint64_t> expected_shape{1, 1};
  BOOST_CHECK_EQUAL_COLLECTIONS(
    shape.begin(), shape.end(),
    expected_shape.begin(), expected_shape.end()
  );
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( get_ndarray_by_path, AsdfFileFixture ) {
  AsdfFile asdf_file(primary_path);
  auto ndarray = asdf_file.getNdarray("PRIMARY");
  BOOST_CHECK_EQUAL(ndarray->ndim(), 2);
  auto shape = ndarray->shape();
  std::vector<uint64_t> expected_shape{1, 1};
  BOOST_CHECK_EQUAL_COLLECTIONS(
    shape.begin(), shape.end(),
    expected_shape.begin(), expected_shape.end()
  );
  BOOST_CHECK_EQUAL(ndarray->getPath(), "/PRIMARY");
}


BOOST_FIXTURE_TEST_CASE( get_fits_wcs_auto, AsdfFileFixture ) {
  AsdfFile asdf_file(wcs_path);
  auto wcs = asdf_file.getFitsWCS();

  BOOST_CHECK(wcs != nullptr);

  auto crpix = wcs->crpix();
  std::array<double, 2> expected_crpix{12099.5, -88700.5};
  for (int idx = 0; idx < 2; idx++) {
    BOOST_CHECK_CLOSE(crpix[idx], expected_crpix[idx], 1e-6);
  }

  auto crval = wcs->crval();
  std::array<double, 2> expected_crval{270., 64.60237301};
  for (int idx = 0; idx < 2; idx++) {
    BOOST_CHECK_CLOSE(crval[idx], expected_crval[idx], 1e-6);
  }

  auto cdelt = wcs->cdelt();
  std::array<double, 2> expected_cdelt{1.52777778e-05, 1.52777778e-05};
  for (int idx = 0; idx < 2; idx++) {
    BOOST_CHECK_CLOSE(cdelt[idx], expected_cdelt[idx], 1e-6);
  }

  auto pc = wcs->pc();
  std::array<std::array<double, 2>, 2> expected_pc{{{1., 0.}, {-0., 1.}}};
  for (int idx = 0; idx < 2; idx++) {
    for (int jdx = 0; jdx < 2; jdx++) {
      BOOST_CHECK_CLOSE(pc[idx][jdx], expected_pc[idx][jdx], 1e-6);
    }
  }

  auto ctype = wcs->ctype();
  std::array<std::string_view, 2> expected_ctype{{"RA---TAN", "DEC--TAN"}};
  BOOST_CHECK_EQUAL_COLLECTIONS(
    ctype.begin(), ctype.end(),
    expected_ctype.begin(), expected_ctype.end()
  );
}

//-----------------------------------------------------------------------------


BOOST_AUTO_TEST_SUITE_END ()
