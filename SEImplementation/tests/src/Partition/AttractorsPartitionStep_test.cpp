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
 * @file tests/src/AttractorsPartitionStep_test.cpp
 * @date 06/03/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Source/SimpleSourceFactory.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEFramework/Pipeline/Partition.h"

#include "SEFramework/Property/DetectionFrame.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStamp.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Partition/AttractorsPartitionStep.h"

using namespace SExtractor;

class DummyCoordinateSystem : public CoordinateSystem {
public:

  virtual WorldCoordinate imageToWorld(ImageCoordinate ) const override {
    return WorldCoordinate(0,0);
  }

  virtual ImageCoordinate worldToImage(WorldCoordinate ) const override {
    return ImageCoordinate(0,0);
  }
};

struct AttractorsPartitionFixture {
  std::shared_ptr<SimpleSource> source {new SimpleSource};
  std::shared_ptr<AttractorsPartitionStep> attractors_step {
    new AttractorsPartitionStep(std::make_shared<SimpleSourceFactory>())
  };
};

class SourceObserver : public Observer<std::shared_ptr<SourceInterface>> {
public:
  virtual void handleMessage(const std::shared_ptr<SourceInterface>& source) override {
      m_list.push_back(source);
  }

  std::list<std::shared_ptr<SourceInterface>> m_list;
};

using namespace SExtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (AttractorsPartitionStep_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( attractors_test, AttractorsPartitionFixture ) {
  auto detection_image = VectorImage<SeFloat>::create(1,1);
  source->setProperty<DetectionFrame>(std::make_shared<DetectionImageFrame>(detection_image, std::make_shared<DummyCoordinateSystem>()));

  source->setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{0,0}, {1,0}, {2,0}, {3,0}});
  source->setProperty<PixelBoundaries>(0, 0, 3, 0);
  auto stamp_one_source = VectorImage<DetectionImage::PixelType>::create(
      4, 1, std::vector<DetectionImage::PixelType> {2.0, 3.0, 4.0, 2.0});
  auto stamp_two_sources = VectorImage<DetectionImage::PixelType>::create(
      4, 1, std::vector<DetectionImage::PixelType> {2.0, 1.0, 1.0, 2.0});

  Partition partition( { attractors_step } );
  auto source_observer = std::make_shared<SourceObserver>();
  partition.addObserver(source_observer);

  source->setProperty<DetectionFrameSourceStamp>(stamp_one_source, nullptr, PixelCoordinate(0,0), nullptr);
  partition.handleMessage(source);
  BOOST_CHECK(source_observer->m_list.size() == 1);
  source_observer->m_list.clear();

  source->setProperty<DetectionFrameSourceStamp>(stamp_two_sources, nullptr, PixelCoordinate(0,0), nullptr);
  partition.handleMessage(source);
  BOOST_CHECK(source_observer->m_list.size() == 2);
  source_observer->m_list.clear();
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
