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

using namespace SourceXtractor;

struct AsdfImageSourceFixture {
  std::string mhdu_path, primary_path, wcs_header, metadata_path, cube_path;

  AsdfImageSourceFixture() {
    mhdu_path = Elements::getAuxiliaryPath("multiple_hdu.asdf").native();
    primary_path = Elements::getAuxiliaryPath("with_primary.asdf").native();
    wcs_header = Elements::getAuxiliaryPath("wcs_header.asdf").native();
    metadata_path = Elements::getAuxiliaryPath("metadata.asdf").native();
    cube_path = Elements::getAuxiliaryPath("cube.asdf").native();
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(AsdfImageSource_test)

//-----------------------------------------------------------------------------

/**
 * A non-square data cube
 *
 * Every other test image was square, 2-D which before this test was added
 * obscured the fact that getWidth()/getHeight() were swapped.
 */
BOOST_FIXTURE_TEST_CASE(cube_dimensions_test, AsdfImageSourceFixture) {
  auto img_src = std::make_shared<AsdfImageSource>(cube_path, "data",
                                                   ImageTile::FloatImage);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 5);
  BOOST_CHECK_EQUAL(img_src->getHeight(), 4);
  BOOST_CHECK_EQUAL(img_src->getDepth(), 3);
}

//-----------------------------------------------------------------------------

/// setLayer picks which layer the tiles come from
BOOST_FIXTURE_TEST_CASE(cube_set_layer_test, AsdfImageSourceFixture) {
  auto img_src = std::make_shared<AsdfImageSource>(cube_path, "data",
                                                   ImageTile::FloatImage);

  for (int layer = 0; layer < 3; layer++) {
    img_src->setLayer(layer);
    auto tile = img_src->getImageTile(0, 0, 5, 4);
    BOOST_CHECK_CLOSE(tile->getValue<SeFloat>(0, 0), layer * 100.f, 1e-8);
    // in an ASDF ndarray (x=4, y=3) is row 3 column 4
    BOOST_CHECK_CLOSE(tile->getValue<SeFloat>(4, 3), layer * 100.f + 34.f, 1e-8);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(cube_rejects_a_missing_layer_test, AsdfImageSourceFixture) {
  auto img_src = std::make_shared<AsdfImageSource>(cube_path, "data",
                                                   ImageTile::FloatImage);
  BOOST_CHECK_THROW(img_src->setLayer(3), Elements::Exception);
  BOOST_CHECK_THROW(img_src->setLayer(-1), Elements::Exception);
}

//-----------------------------------------------------------------------------

/// A two-dimensional ndarray is not a data cube
BOOST_FIXTURE_TEST_CASE(depth_is_zero_without_a_cube_test, AsdfImageSourceFixture) {
  auto img_src = std::make_shared<AsdfImageSource>(metadata_path, "data",
                                                   ImageTile::FloatImage);
  BOOST_CHECK_EQUAL(img_src->getDepth(), 0);
}

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


BOOST_FIXTURE_TEST_CASE(get_metadata, AsdfImageSourceFixture) {
  auto metadata = AsdfImageSource(metadata_path, "data").getMetadata();

  BOOST_CHECK_EQUAL(*metadata->getAs<std::string>("/meta/instrument/optical_element"), "F158");
  BOOST_CHECK_EQUAL(*metadata->getAs<double>("/meta/gain"), 2.5);
  BOOST_CHECK(!metadata->contains("/meta/nope"));

  // getAs is lexical, so an integer keyword also reads as a string
  BOOST_CHECK_EQUAL(*metadata->getAs<std::string>("/meta/saturation"), "65535");

  size_t count = 0;
  metadata->forEach([&count](const std::string&, const MetadataEntry&) {
    count++;
    return true;
  });
  BOOST_CHECK_GT(count, 1u);
}


BOOST_FIXTURE_TEST_CASE(metadata_outlives_the_source, AsdfImageSourceFixture) {
  // The returned Metadata holds the pooled file handle, not the ImageSource, so
  // it keeps working after the source is gone.  That is what the ImageSource
  // interface promises, and what makes the lazy reads safe.
  std::shared_ptr<const Metadata> metadata;

  {
    AsdfImageSource source(metadata_path, "data");
    metadata = source.getMetadata();
  }

  BOOST_CHECK_EQUAL(*metadata->getAs<std::string>("/meta/instrument/name"), "WFI");
}

//-----------------------------------------------------------------------------

// NOTE: WCS selection by name is covered in depth by GWCS_test; here we only
// check that an ASDF image exposes a coordinate system at all -- or, when built
// without libasdf-gwcs, that it reports having none instead of failing.
BOOST_FIXTURE_TEST_CASE(get_coordinate_system, AsdfImageSourceFixture) {
  auto img_src = std::make_shared<AsdfImageSource>(wcs_header);

#ifdef WITH_ASDF_GWCS
  BOOST_CHECK(img_src->getCoordinateSystem() != nullptr);
#else
  BOOST_CHECK(img_src->getCoordinateSystem() == nullptr);
#endif
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

