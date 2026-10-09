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
 * @file tests/src/Image/ImageFileReader_test.cpp
 * @date 10/07/2026
 * @author embray
 */

#include <fstream>

#include <boost/test/unit_test.hpp>

#include <ElementsKernel/Auxiliary.h>
#include <ElementsKernel/Exception.h>
#include <ElementsKernel/Temporary.h>

#include "SEFramework/Image/ImageFileReader.h"

using namespace SourceXtractor;

struct ImageFileReaderFixture {
  std::string fits_path;
  Elements::TempDir temp_dir;

  ImageFileReaderFixture() : temp_dir("ImageFileReader_test_%%%%%%") {
    fits_path = Elements::getAuxiliaryPath("multiple_hdu.fits").native();
  }

  std::string inTempDir(const std::string& name) const {
    return (temp_dir.path() / name).native();
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ImageFileReader_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(create_existing_file, ImageFileReaderFixture) {
  BOOST_CHECK(ImageFileReader::create(fits_path) != nullptr);
}

//-----------------------------------------------------------------------------

/// A path that does not exist must return an accurate error message
BOOST_FIXTURE_TEST_CASE(create_missing_file, ImageFileReaderFixture) {
  auto missing = inTempDir("no_such_image.fits");

  try {
    ImageFileReader::create(missing);
    BOOST_FAIL("expected ImageFileReader::create to throw for a nonexistent file");
  } catch (const Elements::Exception& e) {
    std::string message{e.what()};
    BOOST_CHECK(message.find("does not exist") != std::string::npos);
    BOOST_CHECK(message.find(missing) != std::string::npos);
  }
}

//-----------------------------------------------------------------------------

/// Same for an extension we have no reader for: that one really is an
/// unrecognized format, and must not be confused with the case above
BOOST_FIXTURE_TEST_CASE(create_unknown_format, ImageFileReaderFixture) {
  auto path = inTempDir("not_an_image.xyz");
  std::ofstream{path} << "this is not an image\n";

  try {
    ImageFileReader::create(path);
    BOOST_FAIL("expected ImageFileReader::create to throw for an unrecognized format");
  } catch (const Elements::Exception& e) {
    BOOST_CHECK(std::string{e.what()}.find("could not be determined") != std::string::npos);
  }
}

//-----------------------------------------------------------------------------

/// A directory can be opened by ifstream, so it needs its own check
BOOST_FIXTURE_TEST_CASE(create_directory, ImageFileReaderFixture) {
  try {
    ImageFileReader::create(temp_dir.path().native());
    BOOST_FAIL("expected ImageFileReader::create to throw for a directory");
  } catch (const Elements::Exception& e) {
    BOOST_CHECK(std::string{e.what()}.find("is a directory") != std::string::npos);
  }
}

//-----------------------------------------------------------------------------

/// The [image_path] suffix is stripped before the file is checked, so the
/// diagnostic must name the file rather than the whole argument
BOOST_FIXTURE_TEST_CASE(create_missing_file_with_image_path, ImageFileReaderFixture) {
  auto missing = inTempDir("no_such_image.fits");

  try {
    ImageFileReader::create(missing + "[1]");
    BOOST_FAIL("expected ImageFileReader::create to throw for a nonexistent file");
  } catch (const Elements::Exception& e) {
    std::string message{e.what()};
    BOOST_CHECK(message.find("does not exist") != std::string::npos);
    BOOST_CHECK(message.find(missing + "[1]") == std::string::npos);
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
