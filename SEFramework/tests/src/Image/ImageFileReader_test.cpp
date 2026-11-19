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

#include <boost/filesystem/operations.hpp>
#include <boost/test/unit_test.hpp>

#include <ElementsKernel/Auxiliary.h>
#include <ElementsKernel/Exception.h>
#include <ElementsKernel/Temporary.h>

#include "SEFramework/Image/ImageFileReader.h"

using namespace SourceXtractor;

struct ImageFileReaderFixture {
  std::string fits_path;
#ifdef WITH_ASDF
  std::string asdf_path;
#endif
  Elements::TempDir temp_dir;

  ImageFileReaderFixture() : temp_dir("ImageFileReader_test_%%%%%%") {
    fits_path = Elements::getAuxiliaryPath("multiple_hdu.fits").native();
#ifdef WITH_ASDF
    asdf_path = Elements::getAuxiliaryPath("multiple_hdu.asdf").native();
#endif
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

BOOST_FIXTURE_TEST_CASE(reports_the_format, ImageFileReaderFixture) {
  BOOST_CHECK_EQUAL(ImageFileReader::create(fits_path)->getFormat(), "fits");
}

//-----------------------------------------------------------------------------

/// The format comes from the content, not from the filename extension
/// Regression test; this actually crashed the ASDF content tester if
/// it was passed a FITS file masquerading as ASDF.
BOOST_FIXTURE_TEST_CASE(format_ignores_a_misleading_extension,
                        ImageFileReaderFixture) {
  auto path = inTempDir("actually_fits.asdf");
  boost::filesystem::copy_file(fits_path, path);

  BOOST_CHECK_EQUAL(ImageFileReader::create(path)->getFormat(), "fits");
}

//-----------------------------------------------------------------------------

/// multiple_hdu.fits is [PRIMARY (no data), COMPRESSED_IMAGE, TABLE, IMAGE2]
BOOST_FIXTURE_TEST_CASE(image_names_are_the_extnames, ImageFileReaderFixture) {
  auto names = ImageFileReader::create(fits_path)->getImageNames();

  BOOST_REQUIRE_EQUAL(names.size(), 2);
  BOOST_CHECK_EQUAL(names[0], "COMPRESSED_IMAGE");
  BOOST_CHECK_EQUAL(names[1], "IMAGE2");
}

//-----------------------------------------------------------------------------

/// Each name has to be usable with get(), so the card padding must be gone
BOOST_FIXTURE_TEST_CASE(image_names_resolve, ImageFileReaderFixture) {
  auto reader = ImageFileReader::create(fits_path);

  for (const auto& name : reader->getImageNames()) {
    BOOST_REQUIRE(!name.empty());
    BOOST_CHECK(reader->get(name, ImageTile::FloatImage) != nullptr);
  }
}

//-----------------------------------------------------------------------------

/// A primary HDU has no EXTNAME, and is reachable only by index
BOOST_FIXTURE_TEST_CASE(image_names_empty_for_an_unnamed_hdu,
                        ImageFileReaderFixture) {
  auto path = Elements::getAuxiliaryPath("with_primary.fits").native();
  auto names = ImageFileReader::create(path)->getImageNames();

  BOOST_REQUIRE_EQUAL(names.size(), 1);
  BOOST_CHECK_EQUAL(names[0], "");
}

#ifdef WITH_ASDF

//-----------------------------------------------------------------------------

/// For ASDF the names are the tree paths of the candidate ndarrays, which is
/// the only way to pick one when the file holds more than one
BOOST_FIXTURE_TEST_CASE(asdf_image_names_are_tree_paths, ImageFileReaderFixture) {
  auto names = ImageFileReader::create(asdf_path)->getImageNames();

  BOOST_REQUIRE_EQUAL(names.size(), 2);
  BOOST_CHECK_EQUAL(names[0], "/IMAGE");
  BOOST_CHECK_EQUAL(names[1], "/IMAGE2");
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(reports_the_asdf_format, ImageFileReaderFixture) {
  BOOST_CHECK_EQUAL(ImageFileReader::create(asdf_path)->getFormat(), "asdf");
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(asdf_format_ignores_a_misleading_extension,
                        ImageFileReaderFixture) {
  auto path = inTempDir("actually_asdf.fits");
  boost::filesystem::copy_file(asdf_path, path);

  BOOST_CHECK_EQUAL(ImageFileReader::create(path)->getFormat(), "asdf");
}

//-----------------------------------------------------------------------------

/// The header comment is all that is needed, however short the file is
BOOST_FIXTURE_TEST_CASE(detects_a_short_asdf_file, ImageFileReaderFixture) {
  auto path = inTempDir("tiny.asdf");
  std::ofstream{path} << "#ASDF 1.0.0\n";

  BOOST_CHECK_EQUAL(ImageFileReader::create(path)->getFormat(), "asdf");
}

#endif /* WITH_ASDF */

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

/**
 * With nothing selected, iteration ends at the first HDU that is not an image
 *
 * multiple_hdu.fits is [PRIMARY (no data), COMPRESSED_IMAGE, TABLE, IMAGE2], so
 * iteration yields the one image before the table.  The dataless primary is the
 * single exception, skipped because that is how such a file normally starts.
 *
 * IMAGE2, past the table, is deliberately not reached.  That is a convention,
 * established by the implementation, which I'm not sure was even documented
 * explicitly otherwise.  Nor is a rule of the format.  E.g. HST products
 * interleave tables between science and SQ arrays.
 *
 * This is however the long-standing behavior in SE++ since MEF support was
 * added, so we encode the policy in this test for now, lest we decide to
 * change it later.  See also the comment in FitsReader::get.
 */
BOOST_FIXTURE_TEST_CASE(iter_stops_at_first_non_image, ImageFileReaderFixture) {
  auto reader = ImageFileReader::create(fits_path);

  int image_count = 0;
  for (const auto& img_source : reader->iter(ImageTile::FloatImage)) {
    BOOST_CHECK(img_source != nullptr);
    image_count++;
  }

  BOOST_CHECK_EQUAL(image_count, 1);
}

//-----------------------------------------------------------------------------

/// A file[image] suffix selects the one image it names, so iteration covers that
/// alone rather than every image in the file
BOOST_FIXTURE_TEST_CASE(iter_selected_by_index, ImageFileReaderFixture) {
  auto reader = ImageFileReader::create(fits_path + "[0]");

  int image_count = 0;
  for (const auto& img_source : reader->iter(ImageTile::FloatImage)) {
    BOOST_CHECK(img_source != nullptr);
    image_count++;
  }

  BOOST_CHECK_EQUAL(image_count, 1);
}

//-----------------------------------------------------------------------------

/// setImageName() selects an image for a filename that does not name one itself
BOOST_FIXTURE_TEST_CASE(set_image_name_selects, ImageFileReaderFixture) {
  auto reader = ImageFileReader::create(fits_path);
  reader->setImageName("0");

  int image_count = 0;
  for (const auto& img_source : reader->iter(ImageTile::FloatImage)) {
    BOOST_CHECK(img_source != nullptr);
    image_count++;
  }

  BOOST_CHECK_EQUAL(image_count, 1);
}

//-----------------------------------------------------------------------------

/**
 * A file[image] suffix takes precedence over a configured image path
 *
 * The suffix names one particular file, whereas the configured path typically
 * covers many, so the suffix is the more specific of the two.  Asking for index 99
 * here is what makes the test sharp: were the configured path to win, iteration
 * would throw rather than yield the image the suffix named.
 */
BOOST_FIXTURE_TEST_CASE(set_image_name_yields_to_suffix, ImageFileReaderFixture) {
  auto reader = ImageFileReader::create(fits_path + "[0]");
  reader->setImageName("99");

  int image_count = 0;
  for (const auto& img_source : reader->iter(ImageTile::FloatImage)) {
    BOOST_CHECK(img_source != nullptr);
    image_count++;
  }

  BOOST_CHECK_EQUAL(image_count, 1);
}

//-----------------------------------------------------------------------------

/// A selection that matches nothing is an error, not an empty iteration: there
/// is no other image the user could have meant
BOOST_FIXTURE_TEST_CASE(iter_selected_missing_index, ImageFileReaderFixture) {
  auto reader = ImageFileReader::create(fits_path + "[99]");

  BOOST_CHECK_THROW(
      {
        for (const auto& img_source : reader->iter(ImageTile::FloatImage)) {
          BOOST_CHECK(img_source != nullptr);
        }
      },
      Elements::Exception);
}

//-----------------------------------------------------------------------------

/// Likewise for the first image when nothing was selected: reporting an empty
/// file would hide whatever actually went wrong
BOOST_FIXTURE_TEST_CASE(iter_first_image_failure_propagates, ImageFileReaderFixture) {
  // A FITS file with no readable image at all: valid enough to be recognized by
  // its header, but with nothing for the reader to return
  auto path = inTempDir("header_only.fits");
  {
    std::ofstream out{path, std::ios::binary};
    std::string card(80, ' ');
    card.replace(0, 30, "SIMPLE  =                    T");
    out << card;
    card.assign(80, ' ');
    card.replace(0, 3, "END");
    out << card;
    out << std::string(2880 - 160, ' ');
  }

  auto reader = ImageFileReader::create(path);

  BOOST_CHECK_THROW(
      {
        for (const auto& img_source : reader->iter(ImageTile::FloatImage)) {
          BOOST_CHECK(img_source != nullptr);
        }
      },
      std::exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
