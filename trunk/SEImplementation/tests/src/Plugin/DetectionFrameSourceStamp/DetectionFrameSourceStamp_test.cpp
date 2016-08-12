/**
 * @file tests/src/DetectionFrameSourceStamp_test.cpp
 * @date 06/03/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Image/VectorImage.h"

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
  source.setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{2,0}, {1,1}});
  source.setProperty<PixelBoundaries>(1, 0, 2, 1);

  auto image = std::make_shared<VectorImage<double>>(3, 2, std::vector<double>{0.0, 1.0, 2.0, 3.0, 4.0, 5.0});

  DetectionFrameSourceStampTask task(image);
  task.computeProperties(source);

  auto& source_stamp = source.getProperty<DetectionFrameSourceStamp>().getStamp();

  BOOST_CHECK(source_stamp.getWidth() == 2);
  BOOST_CHECK(source_stamp.getHeight() == 2);
  BOOST_CHECK_CLOSE(source_stamp.getValue(0,0), 1.0, 0.000001);
  BOOST_CHECK_CLOSE(source_stamp.getValue(1,0), 2.0, 0.000001);
  BOOST_CHECK_CLOSE(source_stamp.getValue(0,1), 4.0, 0.000001);
  BOOST_CHECK_CLOSE(source_stamp.getValue(1,1), 5.0, 0.000001);
}

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


