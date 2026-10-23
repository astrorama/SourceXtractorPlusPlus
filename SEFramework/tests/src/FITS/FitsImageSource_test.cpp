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

#include "ElementsKernel/Temporary.h"
#include <ElementsKernel/Auxiliary.h>
#include <ElementsKernel/Real.h>

#include "SEFramework/Image/WriteableBufferedImage.h"
#include "SEFramework/Image/ImageAccessor.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"
#include "SEFramework/CoordinateSystem/WCS.h"
#include "SEFramework/FITS/FitsImageSource.h"

using namespace SourceXtractor;

struct FitsImageSourceFixture {
  std::string mhdu_path, primary_path, wcs_path;
  Elements::TempFile temp_path;

  FitsImageSourceFixture() : temp_path("FitsImageSource_test_%%%%%%.fits") {
    mhdu_path = Elements::getAuxiliaryPath("multiple_hdu.fits").native();
    primary_path = Elements::getAuxiliaryPath("with_primary.fits").native();
    wcs_path = Elements::getAuxiliaryPath("wcs_header.fits").native();
  }
};

/**
 * A CoordinateSystem with no FITS representation, i.e. one that does not
 * implement FitsWcsSerializable.  Stands in for GWCS, which is only available
 * when built against libasdf-gwcs.
 */
class NonSerializableCoordinateSystem : public CoordinateSystem {
public:
  WorldCoordinate imageToWorld(ImageCoordinate image_coordinate) const override {
    return WorldCoordinate(image_coordinate.m_x, image_coordinate.m_y);
  }

  ImageCoordinate worldToImage(WorldCoordinate world_coordinate) const override {
    return ImageCoordinate(world_coordinate.m_alpha, world_coordinate.m_delta);
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FitsImageSource_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(default_is_primary_test, FitsImageSourceFixture) {
  auto img_src = std::make_shared<FitsImageSource>(primary_path, 0, ImageTile::FloatImage);
  BOOST_CHECK_EQUAL(img_src->getHeight(), 1);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 1);
  BOOST_CHECK_CLOSE(img_src->getImageTile(0, 0, 1, 1)->getValue<SeFloat>(0, 0), 1024.44f, 1e-8);
  auto metadata = img_src->getMetadata();
  BOOST_CHECK_EQUAL(boost::get<double>(metadata["GAIN"].m_value), 42);
  BOOST_CHECK_EQUAL(metadata["GAIN"].m_extra["comment"], "ADU");
  BOOST_CHECK_EQUAL(boost::get<std::string>(metadata["STRF"].m_value), "A string 'header'");
  BOOST_CHECK_EQUAL(metadata["STRF"].m_extra["comment"], "Some random stuff");
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(explicit_primary_test, FitsImageSourceFixture) {
  auto img_src = std::make_shared<FitsImageSource>(primary_path + "[0]", 0, ImageTile::FloatImage);
  BOOST_CHECK_EQUAL(img_src->getHeight(), 1);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 1);
  BOOST_CHECK_CLOSE(img_src->getImageTile(0, 0, 1, 1)->getValue<SeFloat>(0, 0), 1024.44f, 1e-8);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(compressed_image_test, FitsImageSourceFixture) {
  auto img_src = std::make_shared<FitsImageSource>(mhdu_path, 0, ImageTile::FloatImage);
  BOOST_CHECK_EQUAL(img_src->getHeight(), 1);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 1);
  BOOST_CHECK_CLOSE(img_src->getImageTile(0, 0, 1, 1)->getValue<SeFloat>(0, 0), 256.2f, 1e-8);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(explicit_compressed_image_test, FitsImageSourceFixture) {
  auto img_src = std::make_shared<FitsImageSource>(mhdu_path + "[1]", 0, ImageTile::FloatImage);
  BOOST_CHECK_EQUAL(img_src->getHeight(), 1);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 1);
  BOOST_CHECK_CLOSE(img_src->getImageTile(0, 0, 1, 1)->getValue<SeFloat>(0, 0), 256.2f, 1e-8);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(second_image_test, FitsImageSourceFixture) {
  auto img_src = std::make_shared<FitsImageSource>(mhdu_path + "[3]", 0, ImageTile::FloatImage);
  BOOST_CHECK_EQUAL(img_src->getHeight(), 1);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 1);
  BOOST_CHECK_CLOSE(img_src->getImageTile(0, 0, 1, 1)->getValue<SeFloat>(0, 0), 1024.44f, 1e-8);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(second_image_by_name_test, FitsImageSourceFixture) {
  auto img_src = std::make_shared<FitsImageSource>(mhdu_path + "[IMAGE2]", 0, ImageTile::FloatImage);
  BOOST_CHECK_EQUAL(img_src->getHeight(), 1);
  BOOST_CHECK_EQUAL(img_src->getWidth(), 1);
  BOOST_CHECK_CLOSE(img_src->getImageTile(0, 0, 1, 1)->getValue<SeFloat>(0, 0), 1024.44f, 1e-8);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(hdu_is_table_test, FitsImageSourceFixture) {
  BOOST_CHECK_THROW(std::make_shared<FitsImageSource>(mhdu_path + "[2]"), Elements::Exception);
  BOOST_CHECK_THROW(std::make_shared<FitsImageSource>(mhdu_path + "[TABLE]"), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(bad_hdu_test, FitsImageSourceFixture) {
  BOOST_CHECK_THROW(std::make_shared<FitsImageSource>(mhdu_path + "[5]"), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(empty_primary_hdu_test, FitsImageSourceFixture) {
  BOOST_CHECK_THROW(std::make_shared<FitsImageSource>(mhdu_path + "[0]"), Elements::Exception);
  BOOST_CHECK_THROW(std::make_shared<FitsImageSource>(mhdu_path + "[PRIMARY]"), Elements::Exception);
}

BOOST_FIXTURE_TEST_CASE(open_write_close_append, FitsImageSourceFixture) {
  // Test creating a FITS file then reopening it to append to it
  {
    auto image_source = std::make_shared<FitsImageSource>(temp_path.path().native(),
        100, 100, ImageTile::FloatImage, nullptr, false, true);
    auto image = WriteableBufferedImage<float>::create(image_source);
    image->setValue(10, 10, 123.f);
  }
  {
    auto image_source = std::make_shared<FitsImageSource>(temp_path.path().native(),
        100, 100, ImageTile::FloatImage, nullptr, true);
    auto image = WriteableBufferedImage<float>::create(image_source);
    ImageAccessor<float> accessor(image);
    BOOST_CHECK_EQUAL(accessor.getValue(10, 10), 0);
    image->setValue(10, 10, 42.f);
  }

  // We need to flush as we don't recognize reopening the same file as being the same ImageSource
  // (we also need a flush at some point to prevent error due to elements deleting the tmp file before destructor)
  TileManager::getInstance()->flush();

  {
    auto image_source = std::make_shared<FitsImageSource>(temp_path.path().native(), 2);
    auto image = BufferedImage<float>::create(image_source);
    ImageAccessor<float> accessor(image);
    BOOST_CHECK_EQUAL(accessor.getValue(10, 10), 123.f);
  }
  {
    auto image_source = std::make_shared<FitsImageSource>(temp_path.path().native(), 3);
    auto image = BufferedImage<float>::create(image_source);
    ImageAccessor<float> accessor(image);
    BOOST_CHECK_EQUAL(accessor.getValue(10, 10), 42.f);
  }
}

BOOST_FIXTURE_TEST_CASE(write_fits_headers, FitsImageSourceFixture) {
  // Test creating a FITS file, writing a custom fits header, then reopening it and reading that header
  {
    auto image_source = std::make_shared<FitsImageSource>(temp_path.path().native(),
        100, 100, ImageTile::FloatImage, nullptr, false, true);
    image_source->setMetadata("TEST", MetadataEntry {std::string("toto"), {}});
  }

  // We need to flush as we don't recognize reopening the same file as being the same ImageSource
  // (we also need a flush at some point to prevent error due to elements deleting the tmp file before destructor)
  TileManager::getInstance()->flush();

  {
    auto image_source = std::make_shared<FitsImageSource>(temp_path.path().native());

    auto metadata = image_source->getMetadata();
    BOOST_CHECK_EQUAL(boost::get<std::string>(metadata["TEST"].m_value), "toto");
  }
}


BOOST_FIXTURE_TEST_CASE(write_wcs_headers, FitsImageSourceFixture) {
  // A coordinate system that can be serialized to FITS gets its cards written
  // out with the image.  This is what gives check images their WCS.
  auto coord_system = FitsImageSource(wcs_path).getCoordinateSystem();

  {
    std::make_shared<FitsImageSource>(temp_path.path().native(), 100, 100,
        ImageTile::FloatImage, coord_system, false, true);
  }

  TileManager::getInstance()->flush();

  {
    auto image_source = std::make_shared<FitsImageSource>(temp_path.path().native());
    auto metadata = image_source->getMetadata();

    BOOST_CHECK_EQUAL(metadata.count("CTYPE1"), 1);
    BOOST_CHECK_EQUAL(metadata.count("CTYPE2"), 1);
    BOOST_CHECK_EQUAL(metadata.count("CRPIX1"), 1);
    BOOST_CHECK_EQUAL(metadata.count("CRVAL1"), 1);

    // Values as they appear in the wcs_header.fits fixture
    BOOST_CHECK_EQUAL(boost::get<std::string>(metadata["CTYPE1"].m_value), "RA---TPV");
    BOOST_CHECK_EQUAL(boost::get<std::string>(metadata["CTYPE2"].m_value), "DEC--TPV");
    BOOST_CHECK_CLOSE(boost::get<double>(metadata["CRVAL1"].m_value), 231.4664282939, 1e-6);
    BOOST_CHECK_CLOSE(boost::get<double>(metadata["CRVAL2"].m_value), 30.48780336375, 1e-6);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(write_without_serializable_wcs, FitsImageSourceFixture) {
  // A coordinate system with no FITS representation is not an error: the image
  // is written without WCS cards rather than failing.
  auto coord_system = std::make_shared<NonSerializableCoordinateSystem>();

  {
    BOOST_CHECK_NO_THROW(std::make_shared<FitsImageSource>(temp_path.path().native(), 100, 100,
        ImageTile::FloatImage, coord_system, false, true));
  }

  TileManager::getInstance()->flush();

  {
    auto image_source = std::make_shared<FitsImageSource>(temp_path.path().native());
    auto metadata = image_source->getMetadata();

    BOOST_CHECK_EQUAL(metadata.count("CTYPE1"), 0);
    BOOST_CHECK_EQUAL(metadata.count("CRPIX1"), 0);
    BOOST_CHECK_EQUAL(metadata.count("CRVAL1"), 0);
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
