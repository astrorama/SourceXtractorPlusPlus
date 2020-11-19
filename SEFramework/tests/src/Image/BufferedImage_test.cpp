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

#include <boost/test/unit_test.hpp>
#include "SEFramework/Image/BufferedImage.h"
#include "SEUtils/TestUtils.h"

using namespace SourceXtractor;


template<typename T>
class ImageSourceMock : public ImageSource {
private:
  std::shared_ptr<Image<T>> m_img;

public:
  ImageSourceMock(const std::shared_ptr<Image<T>> &img) : m_img(img) {}

  virtual ~ImageSourceMock() = default;

  std::string getRepr() const override {
    return "ImageSourceMock(" + m_img->getRepr() + ")";
  }

  void saveTile(ImageTile &) override {
    assert(false);
  }

  int getWidth() const override {
    return m_img->getWidth();
  }

  int getHeight() const override {
    return m_img->getHeight();
  }

  std::shared_ptr<ImageTile> getImageTile(int x, int y, int width, int height) const override {
    auto tile = ImageTile::create(ImageTile::getTypeValue(T()), x, y, width, height);
    for (int iy = y; iy < y + height; ++iy) {
      for (int ix = x; ix < x + width; ++ix) {
        tile->setValue(ix, iy, m_img->getValue(ix, iy));
      }
    }
    return tile;
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

BOOST_AUTO_TEST_SUITE_END ()

