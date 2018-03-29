/**
 * @file tests/src/DetectionFramePixelValues_test.cpp
 * @date 05/27/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValuesTask.h"

using namespace SExtractor;

struct DetectionFramePixelValuesFixture {
  SimpleSource source;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (DetectionFramePixelValues_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(detection_frame_pixel_values_test, DetectionFramePixelValuesFixture) {
  auto image = VectorImage<DetectionImage::PixelType>::create(3, 2,
      std::vector<DetectionImage::PixelType>{0.0, 1.0, 2.0, 3.0, 4.0, 5.0});
  auto variance_map = VectorImage<WeightImage::PixelType>::create(3, 2,
      std::vector<WeightImage::PixelType>{0.00, 0.01, 0.02, 0.03, 0.04, 0.05});

  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{2,0}, {1,1}});
  source.setProperty<DetectionFrame>(std::make_shared<DetectionImageFrame>(
      image,
      nullptr, // no WCS
      variance_map));

  DetectionFramePixelValuesTask task;
  task.computeProperties(source);

  auto& pixel_values = source.getProperty<DetectionFramePixelValues>();
  BOOST_CHECK(pixel_values.getValues() == std::vector<DetectionImage::PixelType>( {2.0, 4.0} ));
  BOOST_CHECK(pixel_values.getVariances() == std::vector<WeightImage::PixelType>( {0.02, 0.04} ));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


