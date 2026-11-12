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
/*
 * BufferedImage_test.cpp
 *
 *  Created on: Feb 18, 2019
 *      Author: Alejandro Álvarez
 */

#include <future>

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/Exception.h"
#include "SEFramework/Image/BufferedImage.h"
#include "SEFramework/Image/WriteableBufferedImage.h"
#include "SEUtils/TestUtils.h"

using namespace SourceXtractor;


template<typename T>
class ImageSourceMock : public ImageSource, public std::enable_shared_from_this<ImageSourceMock<T>> {
private:
  std::shared_ptr<VectorImage<T>> m_img;

public:
  ImageSourceMock(const std::shared_ptr<VectorImage<T>> &img) : m_img(img) {}

  virtual ~ImageSourceMock() = default;

  std::string getRepr() const override {
    return "ImageSourceMock(" + m_img->getRepr() + ")";
  }

  void saveTile(ImageTile& tile) override {
    const auto x = tile.getPosX();
    const auto y = tile.getPosY();
    for (int iy = y; iy < y + tile.getHeight(); ++iy) {
      for (int ix = x; ix < x + tile.getWidth(); ++ix) {
        m_img->setValue(ix, iy, tile.getValue<T>(ix, iy));
      }
    }
  }

  int getWidth() const override {
    return m_img->getWidth();
  }

  int getHeight() const override {
    return m_img->getHeight();
  }

  std::shared_ptr<ImageTile> getImageTile(int x, int y, int width, int height) const override {
    auto tile = ImageTile::create(ImageTile::getTypeValue(T()), x, y, width, height,
                                  std::const_pointer_cast<ImageSourceMock<T>>(this->shared_from_this()));
    for (int iy = y; iy < y + height; ++iy) {
      for (int ix = x; ix < x + width; ++ix) {
        tile->setValue(ix, iy, m_img->getValue(ix, iy));
      }
    }
    return tile;
  }

  ImageTile::ImageType getType() const override {
    return ImageTile::getTypeValue(T());
  }
};

struct BufferedImageFixture {
  std::shared_ptr<ImageSource> m_img_source;

  BufferedImageFixture() {
    m_img_source = std::make_shared<ImageSourceMock<SeFloat>>(VectorImage<SeFloat>::create(
      8, 8, std::vector<SeFloat>{
        0, 0, 1, 1, 2, 2, 3, 3,
        0, 0, 1, 1, 2, 2, 3, 3,
        4, 4, 5, 5, 6, 6, 7, 7,
        4, 4, 5, 5, 6, 6, 7, 7,
        8, 8, 9, 9, 10, 10, 11, 11,
        8, 8, 9, 9, 10, 10, 11, 11,
        12, 12, 13, 13, 14, 14, 15, 15,
        12, 12, 13, 13, 14, 14, 15, 15,
      }
    ));
    TileManager::getInstance()->setOptions(2, 2, 128);
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (BufferedImage_test)

//-----------------------------------------------------------------------------

/**
 * Ask for chunks that align with the tiles
 */
BOOST_FIXTURE_TEST_CASE (ChunkAligned_test, BufferedImageFixture) {
  auto image = BufferedImage<SeFloat>::create(m_img_source);

  auto tile0 = image->getChunk(0, 0, 2, 2);
  BOOST_CHECK(compareImages(VectorImage<SeFloat>::create(2, 2, std::vector<SeFloat>{0, 0, 0, 0}), tile0));

  auto tile1 = image->getChunk(2, 0, 2, 2);
  BOOST_CHECK(compareImages(VectorImage<SeFloat>::create(2, 2, std::vector<SeFloat>{1, 1, 1, 1}), tile1));

  auto tilen = image->getChunk(6, 6, 2, 2);
  BOOST_CHECK(compareImages(VectorImage<SeFloat>::create(2, 2, std::vector<SeFloat>{15, 15, 15, 15}), tilen));
}

//-----------------------------------------------------------------------------

/**
 * Ask for chunks that cross the X boundary between two tiles
 */
BOOST_FIXTURE_TEST_CASE(ChunkCrossX_test, BufferedImageFixture) {
  auto image = BufferedImage<SeFloat>::create(m_img_source);

  auto tile01 = image->getChunk(1, 0, 2, 2);
  BOOST_CHECK(compareImages(VectorImage<SeFloat>::create(2, 2, std::vector<SeFloat>{0, 1, 0, 1}), tile01));

  auto tile56 = image->getChunk(3, 2, 2, 2);
  BOOST_CHECK(compareImages(VectorImage<SeFloat>::create(2, 2, std::vector<SeFloat>{5, 6, 5, 6}), tile56));

  auto tile1415 = image->getChunk(5, 6, 2, 2);
  BOOST_CHECK(compareImages(VectorImage<SeFloat>::create(2, 2, std::vector<SeFloat>{14, 15, 14, 15}), tile1415));
}

//-----------------------------------------------------------------------------

/**
 * Ask for chunks that cross the X boundary across multiple tiles
 */
BOOST_FIXTURE_TEST_CASE(ChunkCrossMultipleX_test, BufferedImageFixture) {
  auto image = BufferedImage<SeFloat>::create(m_img_source);

  auto tile012 = image->getChunk(0, 0, 6, 2);
  BOOST_CHECK(compareImages(
    VectorImage<SeFloat>::create(6, 2,
                                 std::vector<SeFloat>{0, 0, 1, 1, 2, 2,
                                                      0, 0, 1, 1, 2, 2}), tile012));

  auto tile0123 = image->getChunk(1, 0, 6, 2);
  BOOST_CHECK(compareImages(
    VectorImage<SeFloat>::create(6, 2,
                                 std::vector<SeFloat>{0, 1, 1, 2, 2, 3,
                                                      0, 1, 1, 2, 2, 3}), tile0123));

  auto tile4567 = image->getChunk(0, 2, 8, 1);
  BOOST_CHECK(compareImages(
    VectorImage<SeFloat>::create(8, 1,
                                 std::vector<SeFloat>{4, 4, 5, 5, 6, 6, 7, 7}), tile4567));
}

//-----------------------------------------------------------------------------

/**
 * Ask for chunks that cross the Y boundary between two tiles
 */
BOOST_FIXTURE_TEST_CASE(ChunkCrossY_test, BufferedImageFixture) {
  auto image = BufferedImage<SeFloat>::create(m_img_source);

  auto tile04 = image->getChunk(0, 1, 2, 2);
  BOOST_CHECK(compareImages(VectorImage<SeFloat>::create(2, 2, std::vector<SeFloat>{0, 0, 4, 4}), tile04));

  auto tile59 = image->getChunk(2, 3, 2, 2);
  BOOST_CHECK(compareImages(VectorImage<SeFloat>::create(2, 2, std::vector<SeFloat>{5, 5, 9, 9}), tile59));
}

//-----------------------------------------------------------------------------

/**
 * Ask for chunks that cross the Y boundary across multiple tiles
 */
BOOST_FIXTURE_TEST_CASE(ChunkCrossYMultiple_test, BufferedImageFixture) {
  auto image = BufferedImage<SeFloat>::create(m_img_source);

  auto tile048 = image->getChunk(0, 0, 2, 6);
  BOOST_CHECK(compareImages(
    VectorImage<SeFloat>::create(2, 6,
                                 std::vector<SeFloat>{0, 0, 0, 0, 4, 4, 4, 4, 8, 8, 8, 8}), tile048));

  auto tile04812 = image->getChunk(0, 1, 2, 6);
  BOOST_CHECK(compareImages(
    VectorImage<SeFloat>::create(2, 6,
                                 std::vector<SeFloat>{0, 0, 4, 4, 4, 4, 8, 8, 8, 8, 12, 12}), tile04812));

  auto tile261014 = image->getChunk(2, 0, 1, 8);
  BOOST_CHECK(compareImages(
    VectorImage<SeFloat>::create(1, 8,
                                 std::vector<SeFloat>{1, 1, 5, 5, 9, 9, 13, 13}), tile261014));
}

//-----------------------------------------------------------------------------

/**
 * Ask for chunks that cross both X and Y
 */
BOOST_FIXTURE_TEST_CASE(ChunkCrossXY_test, BufferedImageFixture) {
  auto image = BufferedImage<SeFloat>::create(m_img_source);

  auto tile0145 = image->getChunk(1, 1, 2, 2);
  BOOST_CHECK(compareImages(VectorImage<SeFloat>::create(2, 2, std::vector<SeFloat>{0, 1, 4, 5}), tile0145));

  auto tile10111415 = image->getChunk(5, 5, 2, 2);
  BOOST_CHECK(compareImages(VectorImage<SeFloat>::create(2, 2, std::vector<SeFloat>{10, 11, 14, 15}), tile10111415));
}

//-----------------------------------------------------------------------------

/**
 * Ask for chunks that cross both X and Y across multiple tiles
 */
BOOST_FIXTURE_TEST_CASE(ChunkCrossMultipleXY_test, BufferedImageFixture) {
  auto image = BufferedImage<SeFloat>::create(m_img_source);

  auto tile0124568910 = image->getChunk(1, 1, 4, 4);
  BOOST_CHECK(compareImages(
    VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
      0, 1, 1, 2,
      4, 5, 5, 6,
      4, 5, 5, 6,
      8, 9, 9, 10}), tile0124568910));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(WriteAfterEviction) {
  auto manager = std::make_shared<TileManager>();
  manager->setOptions(512, 512, 1);
  auto backing = VectorImage<float>::create(512, 512);
  auto source = std::make_shared<ImageSourceMock<float>>(backing);
  auto image = WriteableBufferedImage<float>::create(source, manager);
  auto other = std::make_shared<ImageSourceMock<float>>(VectorImage<float>::create(512, 512));

  image->setValue(0, 0, 1.f);
  manager->getTileForPixel(0, 0, other);
  BOOST_REQUIRE_EQUAL(backing->getValue(0, 0), 1.f);

  image->setValue(0, 0, 2.f);
  manager->flush();
  BOOST_CHECK_EQUAL(backing->getValue(0, 0), 2.f);
  BOOST_CHECK_EQUAL(image->getChunk(0, 0, 1, 1)->getValue(0, 0), 2.f);
}

BOOST_AUTO_TEST_CASE(WriteBatchSurvivesConcurrentCachePressure) {
  auto manager = std::make_shared<TileManager>();
  manager->setOptions(512, 512, 1);
  auto backing = VectorImage<float>::create(1024, 512);
  auto source = std::make_shared<ImageSourceMock<float>>(backing);
  auto image = WriteableBufferedImage<float>::create(source, manager);
  auto other = std::make_shared<ImageSourceMock<float>>(VectorImage<float>::create(512, 512));

  {
    WriteableImage<float>::WriteBatch batch(*image);
    image->setValue(0, 0, 1.f);
    auto cache_pressure = std::async(std::launch::async, [&] {
      for (int i = 0; i < 8; ++i) {
        manager->getTileForPixel(0, 0, other);
      }
    });
    for (int x = 1; x < 512; ++x) {
      image->setValue(x, 0, 4.f);
    }
    cache_pressure.get();

    BOOST_CHECK_THROW(manager->saveAllTiles(), Elements::Exception);
    BOOST_CHECK_THROW(manager->flush(), Elements::Exception);
    BOOST_CHECK_EQUAL(backing->getValue(0, 0), 0.f);

    image->setValue(0, 0, 2.f);
    BOOST_CHECK_EQUAL(image->getChunk(0, 0, 1, 1)->getValue(0, 0), 2.f);

    image->setValue(512, 0, 3.f);
    BOOST_CHECK_EQUAL(backing->getValue(0, 0), 2.f);
    BOOST_CHECK_EQUAL(backing->getValue(511, 0), 4.f);
    BOOST_CHECK_EQUAL(backing->getValue(512, 0), 0.f);
  }

  manager->flush();
  BOOST_CHECK_EQUAL(backing->getValue(512, 0), 3.f);
}

BOOST_AUTO_TEST_CASE(SaveRejectedBeforeWritingAnyTile) {
  auto manager = std::make_shared<TileManager>();
  manager->setOptions(512, 512, 2);
  auto pinned_backing = VectorImage<float>::create(512, 512);
  auto unpinned_backing = VectorImage<float>::create(512, 512);
  auto pinned_source = std::make_shared<ImageSourceMock<float>>(pinned_backing);
  auto unpinned_source = std::make_shared<ImageSourceMock<float>>(unpinned_backing);
  auto pinned_image = WriteableBufferedImage<float>::create(pinned_source, manager);
  auto unpinned_image = WriteableBufferedImage<float>::create(unpinned_source, manager);

  {
    WriteableImage<float>::WriteBatch batch(*pinned_image);
    pinned_image->setValue(0, 0, 1.f);
    // Insert the unpinned tile last so a save without preflight would visit it first.
    unpinned_image->setValue(0, 0, 2.f);

    BOOST_CHECK_THROW(manager->saveAllTiles(), Elements::Exception);
    BOOST_CHECK_EQUAL(unpinned_backing->getValue(0, 0), 0.f);
    BOOST_CHECK_THROW(manager->flush(), Elements::Exception);
    BOOST_CHECK_EQUAL(unpinned_backing->getValue(0, 0), 0.f);
  }

  manager->flush();
  BOOST_CHECK_EQUAL(pinned_backing->getValue(0, 0), 1.f);
  BOOST_CHECK_EQUAL(unpinned_backing->getValue(0, 0), 2.f);
}

BOOST_AUTO_TEST_CASE(CacheBudgetAllowsMultiplePinnedTiles) {
  auto manager = std::make_shared<TileManager>();
  manager->setOptions(512, 512, 1);
  auto backing_a = VectorImage<float>::create(512, 512);
  auto backing_b = VectorImage<float>::create(512, 512);
  auto source_a = std::make_shared<ImageSourceMock<float>>(backing_a);
  auto source_b = std::make_shared<ImageSourceMock<float>>(backing_b);
  auto image_a = WriteableBufferedImage<float>::create(source_a, manager);
  auto image_b = WriteableBufferedImage<float>::create(source_b, manager);

  {
    WriteableImage<float>::WriteBatch batch_a(*image_a);
    image_a->setValue(0, 0, 1.f);
    {
      WriteableImage<float>::WriteBatch batch_b(*image_b);
      image_b->setValue(0, 0, 2.f);
      BOOST_CHECK_EQUAL(backing_a->getValue(0, 0), 0.f);
      BOOST_CHECK_EQUAL(backing_b->getValue(0, 0), 0.f);
    }
    auto other = std::make_shared<ImageSourceMock<float>>(VectorImage<float>::create(512, 512));
    manager->getTileForPixel(0, 0, other);
    BOOST_CHECK_EQUAL(backing_a->getValue(0, 0), 0.f);
    BOOST_CHECK_EQUAL(backing_b->getValue(0, 0), 2.f);
  }

  manager->saveAllTiles();
  BOOST_CHECK_EQUAL(backing_a->getValue(0, 0), 1.f);
}

BOOST_AUTO_TEST_SUITE_END ()

