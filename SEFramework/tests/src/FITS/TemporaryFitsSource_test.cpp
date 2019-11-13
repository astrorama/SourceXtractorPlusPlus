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
 * @file tests/src/TemporaryFitsSource_test.cpp
 * @date 23/05/18
 * @author aalvarez
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/FITS/TemporaryFitsImageSource.h"

#include <boost/filesystem.hpp>

static const std::string temp_pattern("TemporaryFitsSource_test_%%%%");

using namespace SExtractor;

BOOST_AUTO_TEST_SUITE (TemporaryFitsSource_test)

BOOST_AUTO_TEST_CASE ( automatic_cleanup ) {
  auto temporary_source = std::make_shared<TemporaryFitsImageSource<unsigned int>>(temp_pattern, 10, 10, true);
  auto full_path = temporary_source->getFullPath();

  BOOST_ASSERT(boost::filesystem::exists(full_path));
  BOOST_CHECK_EQUAL(temporary_source->getHeight(), 10);
  BOOST_CHECK_EQUAL(temporary_source->getWidth(), 10);

  temporary_source.reset();

  BOOST_ASSERT(!boost::filesystem::exists(full_path));
}

BOOST_AUTO_TEST_CASE ( keep_file ) {
  auto temporary_source = std::make_shared<TemporaryFitsImageSource<unsigned int>>(temp_pattern, 10, 10, false);
  auto full_path = temporary_source->getFullPath();

  BOOST_ASSERT(boost::filesystem::exists(full_path));

  // Modify some values
  auto tile = temporary_source->getImageTile(0, 0, 5, 5);
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      tile->setValue(i, j, 42);
    }
  }
  tile->setModified(true);
  tile->saveIfModified();

  // Release
  tile.reset();
  temporary_source.reset();

  // The file must exist
  BOOST_ASSERT(boost::filesystem::exists(full_path));

  // If we reopen, the modified content should remain
  auto reopen_source = std::make_shared<FitsImageSource<unsigned int>>(full_path);

  BOOST_CHECK_EQUAL(reopen_source->getHeight(), 10);
  BOOST_CHECK_EQUAL(reopen_source->getWidth(), 10);

  tile = reopen_source->getImageTile(0, 0, 5, 5);
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      BOOST_CHECK_EQUAL(tile->getValue(i, j), 42);
    }
  }

  // Cleanup
  boost::filesystem::remove(full_path);
}

BOOST_AUTO_TEST_SUITE_END ()
