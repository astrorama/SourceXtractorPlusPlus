/**
 * @file tests/src/DetectionFrameSourceStamp_test.cpp
 * @date 06/03/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"

#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStamp.h"
#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStampTask.h"

using namespace SExtractor;

struct DetectionFrameSourceStampFixture {
  SimpleSource source;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (DetectionFrameSourceStamp_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(example_test, DetectionFrameSourceStampFixture) {
  auto image = VectorImage<DetectionImage::PixelType>::create(3, 2,
      std::vector<DetectionImage::PixelType>{0.0, 1.0, 2.0, 3.0, 4.0, 5.0});

  source.setProperty<DetectionFrame>(std::make_shared<DetectionImageFrame>(image));
  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{2,0}, {1,1}});
  source.setProperty<PixelBoundaries>(1, 0, 2, 1);

  DetectionFrameSourceStampTask task;
  task.computeProperties(source);

  auto& source_stamp = source.getProperty<DetectionFrameSourceStamp>().getStamp();
  auto top_left = source.getProperty<DetectionFrameSourceStamp>().getTopLeft();

  // Size must be at least the size of the source itself (+ some area around it)
  BOOST_CHECK(source_stamp.getWidth() >= 2);
  BOOST_CHECK(source_stamp.getHeight() >= 2);

  // Test the 2 pixels that are part of the source
  BOOST_CHECK_CLOSE(source_stamp.getValue(PixelCoordinate(2, 0) - top_left), 2.0, 0.000001);
  BOOST_CHECK_CLOSE(source_stamp.getValue(PixelCoordinate(1, 1) - top_left), 4.0, 0.000001);

  // Test some pixels that are not part of the source but must be included
  // are they are part of the minimal rectangle that includes the source pixel
  BOOST_CHECK_CLOSE(source_stamp.getValue(PixelCoordinate(1, 0) - top_left), 1.0, 0.000001);
  BOOST_CHECK_CLOSE(source_stamp.getValue(PixelCoordinate(2, 1) - top_left), 5.0, 0.000001);
}

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


