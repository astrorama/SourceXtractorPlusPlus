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
 * @file tests/src/Lutz_test.cpp
 * @date 05/31/16
 * @author mschefer
 */


#include "SEImplementation/Segmentation/LutzSegmentation.h"

#include <boost/test/unit_test.hpp>

#include <cmath>
#include <cstdint>
#include <cstring>

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Source/SimpleSourceFactory.h"
#include "SEImplementation/Property/PixelCoordinateList.h"

using namespace SourceXtractor;

class SourceObserver : public Observer<SourceInterface> {
public:
  virtual void handleMessage(const SourceInterface& source) override {
    m_list.push_back(source.getProperty<PixelCoordinateList>());
  }

  std::list<PixelCoordinateList> m_list;
};


struct LutzFixture {
  std::shared_ptr<SourceObserver> source_observer {new SourceObserver};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (Lutz_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( lutz_test, LutzFixture ) {

  // The different groups of continuous pixels we want to get back from Lutz
  std::list<std::vector<DetectionImage::PixelType>> group_images {
    {
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    },
    {
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    },
    {
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

      0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    },
    {
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

      0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    },
    {
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    },
    {
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    },
  };

  // merge them together
  std::vector<DetectionImage::PixelType> composite_image(100, 0.0);
  for (int i = 0; i<100; i++) {
      for (auto& group : group_images) {
        composite_image[i] += group[i];
      }
  }

  auto image = VectorImage<DetectionImage::PixelType>::create(10, 10, composite_image);

  // use Lutz to split them
  Segmentation segmentation(nullptr);
  segmentation.setLabelling<LutzSegmentation>(std::make_shared<SimpleSourceFactory>());

  segmentation.Observable<SourceInterface>::addObserver(source_observer);

  auto detection_frame = std::make_shared<DetectionImageFrame>(image);

  detection_frame->setBackgroundLevel(ConstantImage<DetectionImage::PixelType>::create(image->getWidth(), image->getHeight(), 0), 0.);
  detection_frame->setVarianceMap(ConstantImage<DetectionImage::PixelType>::create(image->getWidth(), image->getHeight(), 0.25));
  segmentation.processFrame(detection_frame);

  // Check that we have the right number of sources
  BOOST_CHECK_EQUAL(source_observer->m_list.size(), group_images.size());

  // For each source that was produced by Lutz, check that it matches one of the groups we had at the start
  // and remove that group
  for (auto& source : source_observer->m_list) {
    auto check_image = VectorImage<DetectionImage::PixelType>::create(10, 10, std::vector<DetectionImage::PixelType>(100, 0.0));
    for (auto& pixel : source.getCoordinateList()) {
      BOOST_CHECK_CLOSE(check_image->getValue(pixel), 0.0, 0.00001);
      check_image->setValue(pixel, 1.0);
    }
    for (auto iter = group_images.begin(); iter != group_images.end(); iter++) {
      if (*iter == check_image->getData()) {
        group_images.erase(iter);
        break;
      }
    }
  }

  // If we had a correct source for every group we should have matched them all and have an empty list
  BOOST_CHECK(group_images.size() == 0);
}

//-----------------------------------------------------------------------------

/// The bit pattern of a 32-bit quiet NaN, read through volatile so that no
/// amount of constant folding can decide the answers below at compile time
volatile uint32_t g_quiet_nan_bits = 0x7FC00000u;

float runtimeNaN() {
  uint32_t bits = g_quiet_nan_bits;
  float value;
  std::memcpy(&value, &bits, sizeof(value));
  return value;
}

/// A NaN is not above the detection threshold, so blank pixels must not be labelled.
/// Under -ffinite-math-only `value > 0.0` in Lutz::labelImage is undefined for a NaN and
/// Clang answers true, which on a real mosaic swallows a whole unexposed region
/// represented by NaN into a single component.
BOOST_FIXTURE_TEST_CASE( lutz_nan_test, LutzFixture ) {
  // Blank the top four rows the way a mosaic blanks its unexposed area, and put a single
  // 3x3 source well clear of it
  DetectionImage::PixelType nan_value = runtimeNaN();

  std::vector<DetectionImage::PixelType> pixels(100, 0.0);
  for (int idx = 0; idx < 40; idx++) {
    pixels[idx] = nan_value;
  }
  for (int y = 6; y <= 8; y++) {
    for (int x = 6; x <= 8; x++) {
      pixels[y * 10 + x] = 1.0;
    }
  }

  auto image = VectorImage<DetectionImage::PixelType>::create(10, 10, pixels);

  Segmentation segmentation(nullptr);
  segmentation.setLabelling<LutzSegmentation>(std::make_shared<SimpleSourceFactory>());
  segmentation.Observable<SourceInterface>::addObserver(source_observer);

  auto detection_frame = std::make_shared<DetectionImageFrame>(image);
  detection_frame->setBackgroundLevel(
      ConstantImage<DetectionImage::PixelType>::create(image->getWidth(), image->getHeight(), 0), 0.);
  detection_frame->setVarianceMap(
      ConstantImage<DetectionImage::PixelType>::create(image->getWidth(), image->getHeight(), 0.25));
  segmentation.processFrame(detection_frame);

  // Only the 3x3 source, with none of the blank pixels dragged in with it
  BOOST_CHECK_EQUAL(source_observer->m_list.size(), 1);

  for (auto& source : source_observer->m_list) {
    auto& coords = source.getCoordinateList();
    BOOST_CHECK_EQUAL(coords.size(), 9u);
    for (auto& pixel : coords) {
      BOOST_CHECK_GE(pixel.m_y, 6);
      BOOST_CHECK_GE(pixel.m_x, 6);
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

