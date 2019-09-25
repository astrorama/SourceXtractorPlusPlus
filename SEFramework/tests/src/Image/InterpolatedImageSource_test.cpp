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
 * @file tests/src/InterpolatedImageSource_test.cpp
 * @date 21/06/19
 * @author Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include "SEFramework/Image/BufferedImage.h"
#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Image/InterpolatedImageSource.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEUtils/TestUtils.h"

using namespace SExtractor;

struct InterpolatedImageSourceFixture {
  std::shared_ptr<VectorImage<SeFloat>> m_image;

  InterpolatedImageSourceFixture() {
    m_image = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
      11., 12., 13., 14.,
      21., 22., 23., 24.,
      31., 32., 33., 34.,
      41., 42., 43., 44.
    });
  }
};

template<typename T, typename ...Args>
std::shared_ptr<BufferedImage<T>> createBufferedHelper(Args&& ...args) {
  return BufferedImage<T>::create(
    std::shared_ptr<InterpolatedImageSource<T>>(new InterpolatedImageSource<T>(std::forward<Args>(args)...))
  );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (InterpolatedImageSource_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImage_all_good_test, InterpolatedImageSourceFixture) {
  auto interpolated = createBufferedHelper<SeFloat>(m_image, ConstantImage<SeFloat>::create(4, 4, 1.), 10., 1);
  BOOST_CHECK(compareImages(interpolated, m_image));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImage_bad_column_test, InterpolatedImageSourceFixture) {
  auto variance = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    1., 1., 5., 1.,
    1., 1., 5., 1.,
    1., 1., 5., 1.,
    1., 1., 5., 1.,
  });
  auto interpolated = createBufferedHelper<SeFloat>(m_image, variance, 2., 1);
  auto expected = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    11., 12., 12., 14.,
    21., 22., 22., 24.,
    31., 32., 32., 34.,
    41., 42., 42., 44.
  });
  BOOST_CHECK(compareImages(interpolated, expected));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImage_bad_first_column_test, InterpolatedImageSourceFixture) {
  auto variance = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    5., 1., 1., 1.,
    5., 1., 1., 1.,
    5., 1., 1., 1.,
    5., 1., 1., 1.,
  });
  auto interpolated = createBufferedHelper<SeFloat>(m_image, variance, 2., 1);
  auto expected = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    11., 12., 13., 14.,
    21., 22., 23., 24.,
    31., 32., 33., 34.,
    41., 42., 43., 44.
  });
  BOOST_CHECK(compareImages(interpolated, expected));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImage_bad_wide_column_test, InterpolatedImageSourceFixture) {
  auto variance = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    1., 5., 5., 1.,
    1., 5., 5., 1.,
    1., 5., 5., 1.,
    1., 5., 5., 1.,
  });
  auto interpolated = createBufferedHelper<SeFloat>(m_image, variance, 2., 2);
  auto expected = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    11., 11., 11., 14.,
    21., 21., 21., 24.,
    31., 31., 31., 34.,
    41., 41., 41., 44.
  });
  BOOST_CHECK(compareImages(interpolated, expected));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImage_bad_row_test, InterpolatedImageSourceFixture) {
  auto variance = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    1., 1., 1., 1.,
    1., 1., 1., 1.,
    5., 5., 5., 5.,
    1., 1., 1., 1.,
  });
  auto interpolated = createBufferedHelper<SeFloat>(m_image, variance, 2., 1);
  auto expected = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    11., 12., 13., 14.,
    21., 22., 23., 24.,
    21., 22., 23., 24.,
    41., 42., 43., 44.
  });
  BOOST_CHECK(compareImages(interpolated, expected));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImage_bad_first_row_test, InterpolatedImageSourceFixture) {
  auto variance = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    5., 5., 5., 5.,
    1., 1., 1., 1.,
    1., 1., 1., 1.,
    1., 1., 1., 1.,
  });
  auto interpolated = createBufferedHelper<SeFloat>(m_image, variance, 2., 1);
  auto expected = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    11., 12., 13., 14.,
    21., 22., 23., 24.,
    31., 32., 33., 34.,
    41., 42., 43., 44.
  });
  BOOST_CHECK(compareImages(interpolated, expected));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImage_bad_wide_row_test, InterpolatedImageSourceFixture) {
  auto variance = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    1., 1., 1., 1.,
    5., 5., 5., 5.,
    5., 5., 5., 5.,
    1., 1., 1., 1.,
  });
  auto interpolated = createBufferedHelper<SeFloat>(m_image, variance, 2., 2);
  auto expected = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    11., 12., 13., 14.,
    11., 12., 13., 14.,
    11., 12., 13., 14.,
    41., 42., 43., 44.
  });
  BOOST_CHECK(compareImages(interpolated, expected));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImage_bad_no_gap_test, InterpolatedImageSourceFixture) {
  auto variance = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    1., 1., 1., 1.,
    1., 1., 5., 1.,
    1., 1., 5., 1.,
    1., 1., 1., 1.,
  });
  auto interpolated = createBufferedHelper<SeFloat>(m_image, variance, 2., 0);
  auto expected = VectorImage<SeFloat>::create(4, 4, std::vector<SeFloat>{
    11., 12., 13., 14.,
    21., 22., 23., 24.,
    31., 32., 33., 34.,
    41., 42., 43., 44.
  });
  BOOST_CHECK(compareImages(interpolated, expected));
}

//-----------------------------------------------------------------------------

template <typename T>
class InterpolatedImageSourceWrapper: public InterpolatedImageSource<T> {
  public:
    mutable int m_tile_count = 0;

    using InterpolatedImageSource<T>::InterpolatedImageSource;

  protected:
  void generateTile(std::shared_ptr<Image<T>> image, ImageTile<T>& tile,
                    int x, int y, int width, int height) const override {
    ++m_tile_count;
    InterpolatedImageSource<T>::generateTile(image, tile, x, y, width, height);
  }
};

struct InterpolatedImageSourceTileFixture {
  std::shared_ptr<VectorImage<SeFloat>> m_image, m_variance, m_expected;
  std::shared_ptr<BufferedImage<SeFloat>> m_buffered;
  std::shared_ptr<TileManager> m_tile_manager;
  std::shared_ptr<InterpolatedImageSourceWrapper<SeFloat>> m_interpolated_source;

  InterpolatedImageSourceTileFixture() {
    m_image = VectorImage<SeFloat>::create(8, 8, std::vector<SeFloat>{
      11., 12., 13., 14.,    15., 16., 17., 18.,
      21., 22., 23., 24.,    25., 26., 27., 28.,
      31., 32., 33., 34.,    35., 36., 37., 38.,
      41., 42., 43., 44.,    45., 46., 47., 48.,

      51., 52., 53., 54.,    55., 56., 57., 58.,
      61., 62., 63., 64.,    65., 66., 67., 68.,
      71., 72., 73., 74.,    75., 76., 77., 78.,
      81., 82., 83., 84.,    85., 86., 87., 88.,
    });
    m_variance = VectorImage<SeFloat>::create(8, 8, std::vector<SeFloat>{
      5., 5., 5., 5.,    5., 0., 0., 0.,
      5., 0., 0., 0.,    5., 0., 0., 0.,
      5., 0., 0., 0.,    5., 0., 0., 0.,
      5., 0., 0., 0.,    5., 0., 0., 0.,

      0., 0., 0., 0.,    5., 5., 5., 5.,
      0., 0., 0., 5.,    0., 0., 5., 0.,
      0., 0., 0., 5.,    0., 0., 5., 0.,
      0., 0., 0., 0.,    0., 0., 5., 0.,
    });
    m_expected = VectorImage<SeFloat>::create(8, 8, std::vector<SeFloat>{
      11., 12., 13., 14.,     15., 16., 17., 18.,
      21., 22., 23., 24.,     24., 26., 27., 28.,
      31., 32., 33., 34.,     34., 36., 37., 38.,
      41., 42., 43., 44.,     44., 46., 47., 48.,

      51., 52., 53., 54.,     54., 46., 47., 48.,
      61., 62., 63., 63.,     65., 66., 66., 68.,
      71., 72., 73., 73.,     75., 76., 76., 78.,
      81., 82., 83., 84.,     85., 86., 86., 88.,
    });
    m_tile_manager = std::make_shared<TileManager>();
    m_tile_manager->setOptions(4, 4, 1024);
    m_interpolated_source = std::make_shared<InterpolatedImageSourceWrapper<SeFloat>>(m_image, m_variance, 2., 1);
    m_buffered = BufferedImage<SeFloat>::create(m_interpolated_source, m_tile_manager);
  }
};

template<typename T, typename ...Args>
std::shared_ptr<BufferedImage<T>> createBufferedHelper(Args&& ...args) {
  return BufferedImage<T>::create(
    std::shared_ptr<InterpolatedImageSource<T>>(new InterpolatedImageSource<T>(std::forward<Args>(args)...))
  );
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (InterpolatedImageSource_tile_full, InterpolatedImageSourceTileFixture) {
  BOOST_CHECK(compareImages(m_expected, m_buffered));
  BOOST_CHECK_EQUAL(m_interpolated_source->m_tile_count, 4);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

//-----------------------------------------------------------------------------
