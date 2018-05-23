/**
 * @file tests/src/TemporaryFitsSource_test.cpp
 * @date 24/05/18
 * @author aalvarez
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Filesystem/TemporaryFile.h"

#include <boost/filesystem.hpp>

static const std::string temp_pattern("TemporaryFile_test_%%%%");

using namespace SExtractor;

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

  temporary_file->touch();

  BOOST_ASSERT(boost::filesystem::exists(full_path));

  temporary_file.reset();

  BOOST_ASSERT(!boost::filesystem::exists(full_path));
}

BOOST_AUTO_TEST_CASE ( keep_file_touch ) {
  auto temporary_file = std::make_shared<TemporaryFile>(false);
  auto full_path = temporary_file->getPath();

  temporary_file->touch() << "TEST_STRING";

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
