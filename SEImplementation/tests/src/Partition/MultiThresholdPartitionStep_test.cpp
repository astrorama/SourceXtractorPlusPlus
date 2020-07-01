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
/*
 * MultiThresholdPartitionStep_test.cpp
 *
 *  Created on: Feb 14, 2017
 *      Author: mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SourceWithOnDemandProperties.h"
#include "SEFramework/Source/SourceWithOnDemandPropertiesFactory.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Task/TaskProvider.h"
#include "SEFramework/Task/TaskFactoryRegistry.h"

#include "SEFramework/Pipeline/Partition.h"

#include "SEFramework/Property/DetectionFrame.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStamp.h"
#include "SEImplementation/Partition/MultiThresholdPartitionStep.h"

#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundariesTaskFactory.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"
#include "SEImplementation/Plugin/PeakValue/PeakValueTaskFactory.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValuesTaskFactory.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroidTaskFactory.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParametersTaskFactory.h"
#include "SEImplementation/Property/SourceId.h"

using namespace SourceXtractor;

class DummyCoordinateSystem : public CoordinateSystem {
public:

  virtual WorldCoordinate imageToWorld(ImageCoordinate ) const override {
    return WorldCoordinate(0,0);
  }

  virtual ImageCoordinate worldToImage(WorldCoordinate ) const override {
    return ImageCoordinate(0,0);
  }
};

struct MultiThresholdPartitionFixture {
  std::shared_ptr<TaskFactoryRegistry> task_factory_registry {new TaskFactoryRegistry};
  std::shared_ptr<TaskProvider> task_provider {new TaskProvider(task_factory_registry)};
  std::shared_ptr<SourceWithOnDemandProperties> source {new SourceWithOnDemandProperties(task_provider)};
  std::shared_ptr<MultiThresholdPartitionStep> multithreshold_step {
    new MultiThresholdPartitionStep(std::make_shared<SourceWithOnDemandPropertiesFactory>(task_provider), 0.005, 32, 1)
  };

  MultiThresholdPartitionFixture() {
    task_factory_registry->registerTaskFactory<ShapeParametersTaskFactory, ShapeParameters>();
    task_factory_registry->registerTaskFactory<PixelCentroidTaskFactory, PixelCentroid>();
    task_factory_registry->registerTaskFactory<DetectionFramePixelValuesTaskFactory, DetectionFramePixelValues>();
    task_factory_registry->registerTaskFactory<PeakValueTaskFactory, PeakValue>();
    task_factory_registry->registerTaskFactory<PixelBoundariesTaskFactory, PixelBoundaries>();
  }
};

class SourceObserver : public Observer<std::shared_ptr<SourceInterface>> {
public:
  virtual void handleMessage(const std::shared_ptr<SourceInterface>& source) override {
      m_list.push_back(source);
  }

  std::list<std::shared_ptr<SourceInterface>> m_list;
};

using namespace SourceXtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (MultiThresholdPartitionStep_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( multithreshold_test, MultiThresholdPartitionFixture ) {
  auto detection_image = VectorImage<SeFloat>::create(4,1);
  detection_image->setValue(0, 0, 2.0);
  detection_image->setValue(1, 0, 3.0);
  detection_image->setValue(2, 0, 4.0);
  detection_image->setValue(3, 0, 1.0);

  source->setProperty<SourceId>();
  source->setProperty<DetectionFrame>(std::make_shared<DetectionImageFrame>(
      detection_image, std::make_shared<DummyCoordinateSystem>()));
  source->setProperty<PeakValue>(1.0, 4.0);

  source->setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{0,0}, {1,0}, {2,0}, {3,0}});
  source->setProperty<PixelBoundaries>(0, 0, 3, 0);

  auto stamp_one_source = VectorImage<DetectionImage::PixelType>::create(
      4, 1, std::vector<DetectionImage::PixelType> {2.0, 3.0, 4.0, 1.0});
  Partition partition( { multithreshold_step } );
  auto source_observer = std::make_shared<SourceObserver>();
  partition.addObserver(source_observer);

  source->setProperty<DetectionFrameSourceStamp>(stamp_one_source, nullptr, nullptr, PixelCoordinate(0,0), nullptr, nullptr);
  partition.handleMessage(source);
  BOOST_CHECK(source_observer->m_list.size() == 1);
}

BOOST_FIXTURE_TEST_CASE( multithreshold_test_2, MultiThresholdPartitionFixture ) {
  auto detection_image = VectorImage<SeFloat>::create(4,1);
  detection_image->setValue(0, 0, 10.0);
  detection_image->setValue(1, 0, 1.0);
  detection_image->setValue(2, 0, 1.0);
  detection_image->setValue(3, 0, 10.0);

  source->setProperty<SourceId>();
  source->setProperty<DetectionFrame>(std::make_shared<DetectionImageFrame>(
      detection_image, std::make_shared<DummyCoordinateSystem>()));
  source->setProperty<PeakValue>(1.0, 10.0);

  source->setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{0,0}, {1,0}, {2,0}, {3,0}});
  source->setProperty<PixelBoundaries>(0, 0, 3, 0);

  auto stamp_one_source = VectorImage<DetectionImage::PixelType>::create(
      4, 1, std::vector<DetectionImage::PixelType> {10.0, 1.0, 1.0, 10.0});
  Partition partition( { multithreshold_step } );
  auto source_observer = std::make_shared<SourceObserver>();
  partition.addObserver(source_observer);

  source->setProperty<DetectionFrameSourceStamp>(stamp_one_source, nullptr, nullptr, PixelCoordinate(0,0), nullptr, nullptr);
  partition.handleMessage(source);
  BOOST_CHECK(source_observer->m_list.size() == 2);
}

BOOST_FIXTURE_TEST_CASE( multithreshold_test_3, MultiThresholdPartitionFixture ) {
  auto detection_image = VectorImage<SeFloat>::create(4,1);
  detection_image->setValue(0, 0, 10.0);
  detection_image->setValue(1, 0, 1.0);
  detection_image->setValue(2, 0, 1.0);
  detection_image->setValue(3, 0, 1.04);

  source->setProperty<SourceId>();
  source->setProperty<DetectionFrame>(std::make_shared<DetectionImageFrame>(
      detection_image, std::make_shared<DummyCoordinateSystem>()));
  source->setProperty<PeakValue>(1.0, 10.0);

  source->setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{0,0}, {1,0}, {2,0}, {3,0}});
  source->setProperty<PixelBoundaries>(0, 0, 3, 0);

  auto stamp_one_source = VectorImage<DetectionImage::PixelType>::create(
      4, 1, std::vector<DetectionImage::PixelType> {10.0, 1.0, 1.0, 1.04});
  Partition partition( { multithreshold_step } );
  auto source_observer = std::make_shared<SourceObserver>();
  partition.addObserver(source_observer);

  source->setProperty<DetectionFrameSourceStamp>(stamp_one_source, nullptr, nullptr, PixelCoordinate(0,0), nullptr, nullptr);
  partition.handleMessage(source);
  BOOST_CHECK(source_observer->m_list.size() == 1);
}
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
