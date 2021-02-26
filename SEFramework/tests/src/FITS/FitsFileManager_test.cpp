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

#if BOOST_VERSION >= 106400
#include <boost/process.hpp>
/// Helper to get the count of open files
/// @warning
///     This seems to be unreliable when running through valgrind
static int countOpenFiles(const std::string& path) {
  namespace bp = boost::process;

  bp::ipstream is;
  bp::child lsof(bp::search_path("lsof"), "-a", "-p", std::to_string(getpid()),
                 path, bp::std_out > is);
  int count = 0;
  std::string line;
  while (lsof.running() && std::getline(is, line) && !line.empty()) {
    ++count;
  }
  // Ignore the header
  return std::max(0, count - 1);
}
#endif

struct FitsImageSourceFixture {
  std::string fits_path;

  FitsImageSourceFixture() {
    fits_path = Elements::getAuxiliaryPath("with_primary.fits").native();
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FitsFileManager_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(OpenOnce_test, FitsImageSourceFixture) {
  auto manager = std::make_shared<FitsFileManager>();
  auto fits = manager->getFitsFile(fits_path);
  fits->open();
  BOOST_CHECK_EQUAL(fits->getImageHdus().size(), 1);
  auto metadata = fits->getHDUHeaders(1);
  BOOST_CHECK_EQUAL(boost::get<double>(metadata["GAIN"].m_value), 42);
#if BOOST_VERSION >= 106400
  BOOST_CHECK_EQUAL(countOpenFiles(fits_path), 1);
#endif
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(OpenReturn_test, FitsImageSourceFixture) {
  auto manager = std::make_shared<FitsFileManager>();
  FitsFile *ptr;
  {
    auto fits = manager->getFitsFile(fits_path);
    fits->open();
    ptr = fits.get();
  }
  auto fits2 = manager->getFitsFile(fits_path);
  // Since it was returned, it should be the same
  BOOST_CHECK_EQUAL(fits2.get(), ptr);
  BOOST_CHECK_EQUAL(fits2->getImageHdus().size(), 1);
  auto metadata = fits2->getHDUHeaders(1);
  BOOST_CHECK_EQUAL(boost::get<double>(metadata["GAIN"].m_value), 42);
#if BOOST_VERSION >= 106400
  BOOST_CHECK_EQUAL(countOpenFiles(fits_path), 1);
#endif
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(OpenTwice_test, FitsImageSourceFixture) {
  auto manager = std::make_shared<FitsFileManager>();
  auto fits = manager->getFitsFile(fits_path);
  fits->open();
  auto fits2 = manager->getFitsFile(fits_path);
  fits2->open();
  // Since it is still open, they should be two different file descriptors
  BOOST_CHECK_NE(fits2->getFitsFilePtr(), fits->getFitsFilePtr());
#if BOOST_VERSION >= 106400
  BOOST_CHECK_EQUAL(countOpenFiles(fits_path), 2);
#endif
}

//-----------------------------------------------------------------------------
#if BOOST_VERSION >= 106400
BOOST_FIXTURE_TEST_CASE(OpenAndClosed_test, FitsImageSourceFixture) {
  auto manager = std::make_shared<FitsFileManager>();
  {
    auto fits = manager->getFitsFile(fits_path);
    fits->open();
    BOOST_CHECK_EQUAL(countOpenFiles(fits_path), 1);
  }

  BOOST_CHECK_EQUAL(countOpenFiles(fits_path), 1);
  manager->closeAllFiles();
  BOOST_CHECK_EQUAL(countOpenFiles(fits_path), 0);
}
#endif
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(FileLimit_test, FitsImageSourceFixture) {
  auto manager = std::make_shared<FitsFileManager>(5);
  std::vector<std::shared_ptr<FitsFile>> hold;
  for (int i = 0; i < 10; ++i) {
    hold.emplace_back(manager->getFitsFile(fits_path));
    hold.back()->open();
  }

  // Even if we are keeping pointers, only 5 must be open
  BOOST_CHECK_EQUAL(hold.size(), 10);
#if BOOST_VERSION >= 106400
  BOOST_CHECK_EQUAL(countOpenFiles(fits_path), 5);
#endif

  // Release all
  manager->closeAllFiles();
#if BOOST_VERSION >= 106400
  BOOST_CHECK_EQUAL(countOpenFiles(fits_path), 0);
#endif

  // But! If now we access one of them, it should work fine
  BOOST_CHECK_EQUAL(hold[0]->getImageHdus().size(), 1);
  auto metadata = hold[0]->getHDUHeaders(1);
  BOOST_CHECK_EQUAL(boost::get<double>(metadata["GAIN"].m_value), 42);

  auto fits = hold[0]->getFitsFilePtr();
  std::vector<double> data(1);
  long start[2] = {1, 1}, end[2] = {1, 1}, increment[2] = {1, 1};
  int status = 0;
  fits_read_subset(fits.get(), TDOUBLE, start, end, increment, nullptr,
                   static_cast<void *>(data.data()), nullptr, &status);
  fits_report_error(stderr, status);
  BOOST_CHECK_EQUAL(status, 0);

#if BOOST_VERSION >= 106400
  BOOST_CHECK_EQUAL(countOpenFiles(fits_path), 1);
#endif
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
