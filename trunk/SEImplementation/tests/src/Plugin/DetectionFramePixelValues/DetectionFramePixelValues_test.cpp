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
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValuesTask.h"

using namespace SExtractor;

struct DetectionFramePixelValuesFixture {
  SimpleSource source;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (DetectionFramePixelValues_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(detection_frame_pixel_values_test, DetectionFramePixelValuesFixture) {
  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{2,0}, {1,1}});

  auto image = std::make_shared<VectorImage<DetectionImage::PixelType>>(3, 2,
      std::vector<DetectionImage::PixelType>{0.0, 1.0, 2.0, 3.0, 4.0, 5.0});

  DetectionFramePixelValuesTask task(image);
  task.computeProperties(source);

  auto& pixel_values = source.getProperty<DetectionFramePixelValues>();
  BOOST_CHECK(pixel_values.getValues() == std::vector<DetectionImage::PixelType>( {2.0, 4.0} ));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


