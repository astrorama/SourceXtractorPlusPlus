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
 * @file tests/src/FitsImageSource_test.cpp
 * @date 11/02/2020
 * @author Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>

#include <ElementsKernel/Auxiliary.h>
#include <ElementsKernel/Real.h>
#include "SEFramework/FITS/FitsImageSource.h"

using namespace SourceXtractor;

struct FitsImageSourceFixture {
  std::string mhdu_path, primary_path;

  FitsImageSourceFixture() {
    mhdu_path = Elements::getAuxiliaryPath("multiple_hdu.fits").native();
    primary_path = Elements::getAuxiliaryPath("with_primary.fits").native();
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FitsImageSource_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(default_is_primary_test, FitsImageSourceFixture) {
  auto img_src = std::make_shared<FitsImageSource<SeFloat>>(primary_path);
  BOOST_CHECK_EQUAL(img_src->getHeight(), 1);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 1);
  BOOST_CHECK_CLOSE(img_src->getImageTile(0, 0, 1, 1)->getValue(0, 0), 1024.44f, 1e-8);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(explicit_primary_test, FitsImageSourceFixture) {
  auto img_src = std::make_shared<FitsImageSource<SeFloat>>(primary_path + "[0]");
  BOOST_CHECK_EQUAL(img_src->getHeight(), 1);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 1);
  BOOST_CHECK_CLOSE(img_src->getImageTile(0, 0, 1, 1)->getValue(0, 0), 1024.44f, 1e-8);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(compressed_image_test, FitsImageSourceFixture) {
  auto img_src = std::make_shared<FitsImageSource<SeFloat>>(mhdu_path);
  BOOST_CHECK_EQUAL(img_src->getHeight(), 1);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 1);
  BOOST_CHECK_CLOSE(img_src->getImageTile(0, 0, 1, 1)->getValue(0, 0), 256.2f, 1e-8);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(explicit_compressed_image_test, FitsImageSourceFixture) {
  auto img_src = std::make_shared<FitsImageSource<SeFloat>>(mhdu_path + "[1]");
  BOOST_CHECK_EQUAL(img_src->getHeight(), 1);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 1);
  BOOST_CHECK_CLOSE(img_src->getImageTile(0, 0, 1, 1)->getValue(0, 0), 256.2f, 1e-8);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(second_image_test, FitsImageSourceFixture) {
  auto img_src = std::make_shared<FitsImageSource<SeFloat>>(mhdu_path + "[3]");
  BOOST_CHECK_EQUAL(img_src->getHeight(), 1);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 1);
  BOOST_CHECK_CLOSE(img_src->getImageTile(0, 0, 1, 1)->getValue(0, 0), 1024.44f, 1e-8);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(second_image_by_name_test, FitsImageSourceFixture) {
  auto img_src = std::make_shared<FitsImageSource<SeFloat>>(mhdu_path + "[IMAGE2]");
  BOOST_CHECK_EQUAL(img_src->getHeight(), 1);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 1);
  BOOST_CHECK_CLOSE(img_src->getImageTile(0, 0, 1, 1)->getValue(0, 0), 1024.44f, 1e-8);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(hdu_is_table_test, FitsImageSourceFixture) {
  BOOST_CHECK_THROW(std::make_shared<FitsImageSource<SeFloat>>(mhdu_path + "[2]"), Elements::Exception);
  BOOST_CHECK_THROW(std::make_shared<FitsImageSource<SeFloat>>(mhdu_path + "[TABLE]"), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(bad_hdu_test, FitsImageSourceFixture) {
  BOOST_CHECK_THROW(std::make_shared<FitsImageSource<SeFloat>>(mhdu_path + "[5]"), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(empty_primary_hdu_test, FitsImageSourceFixture) {
  BOOST_CHECK_THROW(std::make_shared<FitsImageSource<SeFloat>>(mhdu_path + "[0]"), Elements::Exception);
  BOOST_CHECK_THROW(std::make_shared<FitsImageSource<SeFloat>>(mhdu_path + "[PRIMARY]"), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------