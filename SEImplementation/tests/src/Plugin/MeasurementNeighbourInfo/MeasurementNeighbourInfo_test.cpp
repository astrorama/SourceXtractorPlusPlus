/*
 *  MeasurementNeighbourInfo_test.cpp
 *
 *  Created on: Sep 28, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include "SEFramework/Property/DetectionFrame.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Source/SimpleSource.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/DetectionNeighbourInfo/DetectionNeighbourInfo.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementNeighbourInfo/MeasurementNeighbourInfo.h"
#include "SEImplementation/Plugin/MeasurementNeighbourInfo/MeasurementNeighbourInfoTask.h"
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
  PixelCoordinate m_center;
  float m_rads;

  RotateCoordinateSystem(PixelCoordinate center, float rads = M_PI_2) : m_center{center}, m_rads{rads} {}

  virtual WorldCoordinate imageToWorld(ImageCoordinate i) const override {
    return WorldCoordinate{
      (i.m_x - m_center.m_x) * cos(m_rads) - (i.m_y - m_center.m_y) * sin(m_rads) + m_center.m_x,
      (i.m_x - m_center.m_x) * sin(m_rads) + (i.m_y - m_center.m_y) * cos(m_rads) + m_center.m_y
    };
  }

  virtual ImageCoordinate worldToImage(WorldCoordinate w) const override {
    throw Elements::Exception() << "Call not expected";
  }
};


struct MeasurementNeighbourInfo_Fixture {
  std::shared_ptr<Image<int>> detection_neighbour_img = VectorImage<int>::create(3, 3, std::vector<int>{
    0, 0, 1,
    0, 0, 0,
    0, 0, 0,
  });

  SimpleSource source;

  MeasurementNeighbourInfo_Fixture() {
    source.setProperty<PixelBoundaries>(1, 1, 3, 3);
    source.setProperty<DetectionFrame>(std::make_shared<DetectionImageFrame>(
      nullptr, nullptr, 0., std::make_shared<DummyCoordinateSystem>(), 0., 60000, 0
    ));
    source.setProperty<DetectionNeighbourInfo>(PixelCoordinate{1, 1}, detection_neighbour_img);
  }
};

BOOST_AUTO_TEST_SUITE(NeighbourInfo_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(OneToOne_test, MeasurementNeighbourInfo_Fixture) {
  MeasurementNeighbourInfoTask task{0};

  source.setProperty<MeasurementFrame>(std::make_shared<MeasurementImageFrame>(
    nullptr, nullptr, 0., std::make_shared<DummyCoordinateSystem>(), 0., 6000., 0
  ));
  source.setProperty<MeasurementFrameRectangle>(PixelCoordinate{1, 1}, PixelCoordinate{3, 3});

  task.computeProperties(source);

  auto neighbour_info = source.getProperty<MeasurementNeighbourInfo>().getImage();

  auto expected = VectorImage<int>::create(3, 3, std::vector<int>{
    0, 0, 1,
    0, 0, 0,
    0, 0, 0,
  });

  BOOST_CHECK(compareImages(expected, neighbour_info));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(Rotate_test, MeasurementNeighbourInfo_Fixture) {
  MeasurementNeighbourInfoTask task{0};

  source.setProperty<MeasurementFrame>(std::make_shared<MeasurementImageFrame>(
    nullptr, nullptr, 0., std::make_shared<RotateCoordinateSystem>(PixelCoordinate{2, 2}), 0., 6000., 0
  ));
  source.setProperty<MeasurementFrameRectangle>(PixelCoordinate{1, 1}, PixelCoordinate{3, 3});

  task.computeProperties(source);

  auto neighbour_info = source.getProperty<MeasurementNeighbourInfo>().getImage();

  auto expected = VectorImage<int>::create(3, 3, std::vector<int>{
    1, 0, 0,
    0, 0, 0,
    0, 0, 0,
  });

  BOOST_CHECK(compareImages(expected, neighbour_info));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
