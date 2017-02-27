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
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Partition/MultiThresholdPartitionStep.h"

#include "SEImplementation/Plugin/PeakValue/PeakValue.h"
#include "SEImplementation/Plugin/PeakValue/PeakValueTaskFactory.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValuesTaskFactory.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroidTaskFactory.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParametersTaskFactory.h"

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

struct MultiThresholdPartitionFixture {
  std::shared_ptr<TaskFactoryRegistry> task_factory_registry {new TaskFactoryRegistry};
  std::shared_ptr<TaskProvider> task_provider {new TaskProvider(task_factory_registry)};
  std::shared_ptr<SourceWithOnDemandProperties> source {new SourceWithOnDemandProperties(task_provider)};
  std::shared_ptr<MultiThresholdPartitionStep> attractors_step {
    new MultiThresholdPartitionStep(std::make_shared<SourceWithOnDemandPropertiesFactory>(task_provider))
  };

  MultiThresholdPartitionFixture() {
    task_factory_registry->registerTaskFactory<ShapeParametersTaskFactory, ShapeParameters>();
    task_factory_registry->registerTaskFactory<PixelCentroidTaskFactory, PixelCentroid>();
    task_factory_registry->registerTaskFactory<DetectionFramePixelValuesTaskFactory, DetectionFramePixelValues>();
    task_factory_registry->registerTaskFactory<PeakValueTaskFactory, PeakValue>();
  }
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

BOOST_FIXTURE_TEST_CASE( multithreshold_test, MultiThresholdPartitionFixture ) {
  auto detection_image = std::make_shared<VectorImage<SeFloat>>(4,1);
  detection_image->setValue(0, 0, 2.0);
  detection_image->setValue(1, 0, 3.0);
  detection_image->setValue(2, 0, 4.0);
  detection_image->setValue(3, 0, 1.0);

  source->setProperty<DetectionFrame>(detection_image, detection_image, std::make_shared<DummyCoordinateSystem>());
  source->setProperty<PeakValue>(1.0, 4.0);

  source->setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{0,0}, {1,0}, {2,0}, {3,0}});
  source->setProperty<PixelBoundaries>(0, 0, 3, 0);

  auto stamp_one_source = std::make_shared<VectorImage<DetectionImage::PixelType>>(
      4, 1, std::vector<DetectionImage::PixelType> {2.0, 3.0, 4.0, 1.0});
  Partition partition( { attractors_step } );
  auto source_observer = std::make_shared<SourceObserver>();
  partition.addObserver(source_observer);

  source->setProperty<DetectionFrameSourceStamp>(stamp_one_source);
  partition.handleMessage(source);
  BOOST_CHECK(source_observer->m_list.size() == 1);
}

BOOST_FIXTURE_TEST_CASE( multithreshold_test_2, MultiThresholdPartitionFixture ) {
  auto detection_image = std::make_shared<VectorImage<SeFloat>>(4,1);
  detection_image->setValue(0, 0, 10.0);
  detection_image->setValue(1, 0, 1.0);
  detection_image->setValue(2, 0, 1.0);
  detection_image->setValue(3, 0, 10.0);

  source->setProperty<DetectionFrame>(detection_image, detection_image, std::make_shared<DummyCoordinateSystem>());
  source->setProperty<PeakValue>(1.0, 10.0);

  source->setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{0,0}, {1,0}, {2,0}, {3,0}});
  source->setProperty<PixelBoundaries>(0, 0, 3, 0);

  auto stamp_one_source = std::make_shared<VectorImage<DetectionImage::PixelType>>(
      4, 1, std::vector<DetectionImage::PixelType> {10.0, 1.0, 1.0, 10.0});
  Partition partition( { attractors_step } );
  auto source_observer = std::make_shared<SourceObserver>();
  partition.addObserver(source_observer);

  source->setProperty<DetectionFrameSourceStamp>(stamp_one_source);
  partition.handleMessage(source);
  BOOST_CHECK(source_observer->m_list.size() == 2);
}

BOOST_FIXTURE_TEST_CASE( multithreshold_test_3, MultiThresholdPartitionFixture ) {
  auto detection_image = std::make_shared<VectorImage<SeFloat>>(4,1);
  detection_image->setValue(0, 0, 10.0);
  detection_image->setValue(1, 0, 1.0);
  detection_image->setValue(2, 0, 1.0);
  detection_image->setValue(3, 0, 1.04);

  source->setProperty<DetectionFrame>(detection_image, detection_image, std::make_shared<DummyCoordinateSystem>());
  source->setProperty<PeakValue>(1.0, 10.0);

  source->setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{0,0}, {1,0}, {2,0}, {3,0}});
  source->setProperty<PixelBoundaries>(0, 0, 3, 0);

  auto stamp_one_source = std::make_shared<VectorImage<DetectionImage::PixelType>>(
      4, 1, std::vector<DetectionImage::PixelType> {10.0, 1.0, 1.0, 1.04});
  Partition partition( { attractors_step } );
  auto source_observer = std::make_shared<SourceObserver>();
  partition.addObserver(source_observer);

  source->setProperty<DetectionFrameSourceStamp>(stamp_one_source);
  partition.handleMessage(source);
  BOOST_CHECK(source_observer->m_list.size() == 1);
}
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
