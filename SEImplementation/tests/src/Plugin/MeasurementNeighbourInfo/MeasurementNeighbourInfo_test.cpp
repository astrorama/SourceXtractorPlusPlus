/*
 *  NeighbourInfo_test.cpp
 *
 *  Created on: Sep 28, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include "SEFramework/Property/DetectionFrame.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Source/SimpleSource.h"
#include "SEImplementation/Plugin/NeighbourInfo/NeighbourInfoTask.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/NeighbourInfo/NeighbourInfo.h"
#include "SEImplementation/Plugin/MeasurementFrameRectangle/MeasurementFrameRectangle.h"

#include "SEFramework/tests/src/Image/CompareImages.h"

using namespace SExtractor;


class DummyCoordinateSystem : public CoordinateSystem {
public:

  virtual WorldCoordinate imageToWorld(ImageCoordinate i) const override {
    return WorldCoordinate(i.m_x, i.m_y);
  }

  virtual ImageCoordinate worldToImage(WorldCoordinate w) const override {
    return ImageCoordinate(w.m_alpha, w.m_delta);
  }
};

class RotateCoordinateSystem : public CoordinateSystem {
public:
  float m_rads;
  PixelCoordinate m_center;

  RotateCoordinateSystem(PixelCoordinate center, float rads = M_PI_2) : m_center{center}, m_rads{rads} {}

  virtual WorldCoordinate imageToWorld(ImageCoordinate i) const override {
    return WorldCoordinate{
      (i.m_x - m_center.m_x) * cos(m_rads) - (i.m_y - m_center.m_y) * sin(m_rads) + m_center.m_x,
      (i.m_x - m_center.m_x) * sin(m_rads) + (i.m_y - m_center.m_y) * cos(m_rads) + m_center.m_y
    };
    /*return WorldCoordinate(
      i.m_x * cos(m_rads) - i.m_y * sin(m_rads) + 2,
      i.m_x * sin(m_rads) + i.m_y * cos(m_rads)
    );*/
  }

  virtual ImageCoordinate worldToImage(WorldCoordinate w) const override {
    throw Elements::Exception() << "Call not expected";
  }
};


struct MeasurementNeighbourInfo_Fixture {
  // Two "sources", once centered at 2,2 (42), and another one
  // at 4,2 (24). Tests will run on the centered one, whose pixels
  // are identified by detection_coordinates.
  std::shared_ptr<Image<float>> detection_image = VectorImage<SeFloat>::create(5, 5, std::vector<float>{
    0.,  0.,  0.,  0.,  8.,
    0.,  6., 12., 12., 24.,
    0., 12., 42., 16.,  8.,
    0.,  6., 12.,  6.,  0.,
    0.,  0.,  0.,  0.,  0.,
  });

  std::shared_ptr<Image<float>> variance_map = ConstantImage<float>::create(5, 5, .0001);

  std::vector<PixelCoordinate> detection_pixel_list{
    {1, 1},
    {2, 1},
    {1, 2},
    {2, 2},
    {3, 2},
    {1, 3},
    {2, 3},
    {3, 3},
  };

  SimpleSource source;

  NeighbourInfo_Fixture() {
    source.setProperty<PixelCoordinateList>(detection_pixel_list);
    source.setProperty<PixelBoundaries>(1, 1, 3, 3);
    source.setProperty<DetectionFrame>(std::make_shared<DetectionImageFrame>(
      detection_image, variance_map, 0., std::make_shared<DummyCoordinateSystem>(), 0., 60000, 0
    ));
  }
};

BOOST_AUTO_TEST_SUITE(NeighbourInfo_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(OneToOne_test, MeasurementNeighbourInfo_Fixture) {
  NeighbourInfoTask task{0};

  source.setProperty<MeasurementFrame>(std::make_shared<MeasurementImageFrame>(
    detection_image, variance_map, 0., std::make_shared<DummyCoordinateSystem>(), 0., 6000., 0
  ));
  source.setProperty<MeasurementFrameRectangle>(PixelCoordinate{1, 1}, PixelCoordinate{3, 3});

  task.computeProperties(source);

  auto neighbour_info = source.getProperty<NeighbourInfo>().getImage();

  auto expected = VectorImage<int>::create(3, 3, std::vector<int>{
    0, 0, 1,
    0, 0, 0,
    0, 0, 0,
  });

  BOOST_CHECK(compareImages(expected, neighbour_info));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(Rotate_test, MeasurementNeighbourInfo_Fixture) {
  NeighbourInfoTask task{0};

  source.setProperty<MeasurementFrame>(std::make_shared<MeasurementImageFrame>(
    detection_image, variance_map, 0., std::make_shared<RotateCoordinateSystem>(PixelCoordinate{2, 2}), 0., 6000., 0
  ));
  source.setProperty<MeasurementFrameRectangle>(PixelCoordinate{1, 1}, PixelCoordinate{3, 3});

  task.computeProperties(source);

  auto neighbour_info = source.getProperty<NeighbourInfo>().getImage();

  auto expected = VectorImage<int>::create(3, 3, std::vector<int>{
    1, 0, 0,
    0, 0, 0,
    0, 0, 0,
  });

  BOOST_CHECK(compareImages(expected, neighbour_info));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
