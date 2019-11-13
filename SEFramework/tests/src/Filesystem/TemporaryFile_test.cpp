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
 * @date 24/05/18
 * @author aalvarez
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Filesystem/TemporaryFile.h"

#include <fstream>
#include <boost/filesystem.hpp>

static const std::string temp_pattern("TemporaryFile_test_%%%%");

using namespace SourceXtractor;

BOOST_AUTO_TEST_SUITE (TemporaryFile_test)

BOOST_AUTO_TEST_CASE ( automatic_cleanup_not_created ) {
  auto temporary_file = std::make_shared<TemporaryFile>(true);
  auto full_path = temporary_file->getPath();

  BOOST_ASSERT(!boost::filesystem::exists(full_path));

  temporary_file.reset();

  BOOST_ASSERT(!boost::filesystem::exists(full_path));
}

BOOST_AUTO_TEST_CASE ( keep_file_not_created ) {
  auto temporary_file = std::make_shared<TemporaryFile>(false);
  auto full_path = temporary_file->getPath();

  BOOST_ASSERT(!boost::filesystem::exists(full_path));

  temporary_file.reset();

  BOOST_ASSERT(!boost::filesystem::exists(full_path));
  boost::filesystem::remove(full_path);
}

BOOST_AUTO_TEST_CASE ( automatic_cleanup_touch ) {
  auto temporary_file = std::make_shared<TemporaryFile>(true);
  auto full_path = temporary_file->getPath();

  std::ofstream out(full_path.c_str());
  out.close();

  BOOST_ASSERT(boost::filesystem::exists(full_path));

  temporary_file.reset();

  BOOST_ASSERT(!boost::filesystem::exists(full_path));
}

BOOST_AUTO_TEST_CASE ( keep_file_touch ) {
  auto temporary_file = std::make_shared<TemporaryFile>(false);
  auto full_path = temporary_file->getPath();

  std::ofstream out(full_path.c_str());
  out << "TEST_STRING";
  out.close();

  BOOST_ASSERT(boost::filesystem::exists(full_path));

  temporary_file.reset();

  BOOST_ASSERT(boost::filesystem::exists(full_path));

  std::ifstream fd(full_path);
  std::string str;
  fd >> str;
  BOOST_CHECK_EQUAL(str, "TEST_STRING");

  boost::filesystem::remove(full_path);
}

BOOST_AUTO_TEST_SUITE_END ()
