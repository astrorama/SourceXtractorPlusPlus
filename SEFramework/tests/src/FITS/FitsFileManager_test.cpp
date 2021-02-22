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

#include <boost/test/unit_test.hpp>
#include <ElementsKernel/Auxiliary.h>
#include "SEFramework/FITS/FitsFile.h"
#include "SEFramework/FITS/FitsFileManager.h"

using namespace SourceXtractor;

struct FitsImageSourceFixture {
  std::string mhdu_path;

  FitsImageSourceFixture() {
    mhdu_path = Elements::getAuxiliaryPath("multiple_hdu.fits").native();
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FitsFileManager_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(OpenOnce_test, FitsImageSourceFixture) {
  auto manager = std::make_shared<FitsFileManager>();
  auto fits = manager->getFitsFile(mhdu_path);
  fits->open();
  BOOST_CHECK_EQUAL(fits->getImageHdus().size(), 2);
  auto metadata = fits->getHDUHeaders(2);
  BOOST_CHECK_EQUAL(boost::get<std::string>(metadata["EXTNAME"].m_value), "COMPRESSED_IMAGE");
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(OpenReturn_test, FitsImageSourceFixture) {
  auto manager = std::make_shared<FitsFileManager>();
  fitsfile *ptr;
  {
    auto fits = manager->getFitsFile(mhdu_path);
    ptr = fits->getFitsFilePtr();
  }
  BOOST_CHECK_EQUAL(manager->count(), 1);
  auto fits2 = manager->getFitsFile(mhdu_path);
  // Since it was returned, it should be the same
  BOOST_CHECK_EQUAL(fits2->getFitsFilePtr(), ptr);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(OpenTwice_test, FitsImageSourceFixture) {
  auto manager = std::make_shared<FitsFileManager>();
  auto fits = manager->getFitsFile(mhdu_path);
  auto fits2 = manager->getFitsFile(mhdu_path);
  // Since it is still open, they should be two different file descriptors
  BOOST_CHECK_NE(fits2->getFitsFilePtr(), fits->getFitsFilePtr());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(OpenAndClosed_test, FitsImageSourceFixture) {
  auto manager = std::make_shared<FitsFileManager>();
  {
    auto fits = manager->getFitsFile(mhdu_path);
    fits->close();
  }

  BOOST_CHECK_EQUAL(manager->count(), 0);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(FileLimit_test, FitsImageSourceFixture) {
  auto manager = std::make_shared<FitsFileManager>(5);
  std::vector<std::shared_ptr<FitsFile>> hold;
  for (int i = 0; i < 10; ++i) {
    hold.emplace_back(manager->getFitsFile(mhdu_path));
    hold.back()->open();
  }
  BOOST_CHECK_EQUAL(hold.size(), 10);
  // All are outside
  BOOST_CHECK_EQUAL(manager->count(), 0);

  // Release all
  hold.resize(0);

  BOOST_CHECK_EQUAL(manager->count(), 5);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
