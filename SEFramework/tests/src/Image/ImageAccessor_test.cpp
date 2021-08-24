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
#include "SEFramework/Image/ImageAccessor.h"
#include "SEFramework/Image/VectorImage.h"

using namespace SourceXtractor;

template<typename T>
class MockImage : public Image<T> {
public:
  MockImage(std::shared_ptr<VectorImage<T>>& img) : m_decorated(img), m_count(0) {}

  std::string getRepr() const override {
    return "Decorated";
  }

  T getValue(int x, int y) const {
    return m_decorated->getValue(x, y);
  }

  int getWidth() const override {
    return m_decorated->getWidth();
  }

  int getHeight() const override {
    return m_decorated->getHeight();
  }

  std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const override {
    ++m_count;
    return m_decorated->getChunk(x, y, width, height);
  }

  int getChunkCount() const {
    return m_count;
  }

private:
  mutable int m_count;
  std::shared_ptr<VectorImage<T>> m_decorated;
};

struct ImageAccessor_Fixture {
  std::shared_ptr<MockImage<float>> m_image;

  ImageAccessor_Fixture() {
    auto vimg = VectorImage<float>::create(512, 512);
    std::generate(vimg->begin(), vimg->end(), std::rand);

    m_image = std::make_shared<MockImage<float>>(vimg);
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ImageAccessor_test)

//-----------------------------------------------------------------------------

// Default constructor, typical access pattern from top left to bottom right
BOOST_FIXTURE_TEST_CASE(AccessorDefault_test, ImageAccessor_Fixture) {
  ImageAccessor<float> accessor(m_image);

  for (int iy = 0; iy < m_image->getHeight(); ++iy) {
    for (int ix = 0; ix < m_image->getWidth(); ++ix) {
      BOOST_CHECK_EQUAL(accessor.getValue(ix, iy), m_image->getValue(ix, iy));
    }
  }

  // Can't do better than read as many chunks as the width multiplied by the height
  BOOST_CHECK_EQUAL(m_image->getChunkCount(), (m_image->getWidth() / 64.) * m_image->getHeight());
}

//-----------------------------------------------------------------------------

// Access from bottom right to top left
BOOST_FIXTURE_TEST_CASE(AccessorReversed_test, ImageAccessor_Fixture) {
  ImageAccessor<float> accessor(m_image, ImageAccessor<float>::BOTTOM_RIGHT);

  for (int iy = m_image->getHeight() - 1; iy >= 0; --iy) {
    for (int ix = m_image->getWidth() - 1; ix >= 0; --ix) {
      BOOST_CHECK_EQUAL(accessor.getValue(ix, iy), m_image->getValue(ix, iy));
    }
  }

  BOOST_CHECK_LE(m_image->getChunkCount(), (m_image->getWidth() / 64.) * m_image->getHeight());
}

//-----------------------------------------------------------------------------

// Give a hint that we will read around the pixel, so a single access is required
BOOST_FIXTURE_TEST_CASE(AccessorCentered_test, ImageAccessor_Fixture) {
  ImageAccessor<float> accessor(m_image, ImageAccessor<float>::CENTERED, 64, 64);

  int x = 128, y = 47;
  for (int r = 0; r < 32; ++r) {
    BOOST_CHECK_EQUAL(accessor.getValue(x - r, y), m_image->getValue(x - r, y));
    BOOST_CHECK_EQUAL(accessor.getValue(x + r, y), m_image->getValue(x + r, y));
    BOOST_CHECK_EQUAL(accessor.getValue(x, y - r), m_image->getValue(x, y - r));
    BOOST_CHECK_EQUAL(accessor.getValue(x, y + r), m_image->getValue(x, y + r));
  }

  BOOST_CHECK_EQUAL(m_image->getChunkCount(), 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
