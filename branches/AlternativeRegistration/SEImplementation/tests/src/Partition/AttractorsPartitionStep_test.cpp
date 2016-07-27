/**
 * @file tests/src/AttractorsPartitionStep_test.cpp
 * @date 06/03/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/Source.h"
#include "SEFramework/Task/TaskRegistry.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEFramework/Pipeline/Partition.h"

#include "SEImplementation/Property/DetectionFrameSourceStamp.h"
#include "SEImplementation/Task/PixelBoundariesTaskFactory.h"
#include "SEImplementation/Partition/AttractorsPartitionStep.h"

using namespace SExtractor;

struct AttractorsPartitionFixture {
  std::shared_ptr<TaskRegistry> task_registry;
  std::shared_ptr<AttractorsPartitionStep> attractors_step;
  std::shared_ptr<Source> source;

  AttractorsPartitionFixture()
      : task_registry(new TaskRegistry()),
        attractors_step(new AttractorsPartitionStep(task_registry)) {
    task_registry->registerTaskFactory(std::unique_ptr<TaskFactory>(new PixelBoundariesTaskFactory));
    //task_registry->registerTaskFactory(std::unique_ptr<TaskFactory>(new cTaskFactory));
  }
};

class SourceObserver : public Observer<std::shared_ptr<Source>> {
public:
  virtual void handleMessage(const std::shared_ptr<Source>& source) override {
      m_list.push_back(source);
  }

  std::list<std::shared_ptr<Source>> m_list;
};

using namespace SExtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (AttractorsPartitionStep_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( attractors_test, AttractorsPartitionFixture ) {
  source.reset(new Source(
      std::vector<PixelCoordinate>( {
        PixelCoordinate(0,0), PixelCoordinate(1,0),
        PixelCoordinate(2,0), PixelCoordinate(3,0)
  } ), task_registry));

  auto stamp_one_source = std::make_shared<VectorImage<double>>(4, 1, std::vector<double> {2.0, 3.0, 4.0, 2.0});
  auto stamp_two_sources = std::make_shared<VectorImage<double>>(4, 1, std::vector<double> {2.0, 1.0, 1.0, 2.0});

  Partition partition( { attractors_step } );
  auto source_list_observer = std::make_shared<SourceObserver>();
  partition.addObserver(source_list_observer);

  source->setProperty(std::unique_ptr<DetectionFrameSourceStamp>(new DetectionFrameSourceStamp(stamp_one_source)));
  partition.handleMessage(source);
  BOOST_CHECK(source_list_observer->m_list.size() == 1);
  source_list_observer->m_list.clear();

  source->setProperty(std::unique_ptr<DetectionFrameSourceStamp>(new DetectionFrameSourceStamp(stamp_two_sources)));
  partition.handleMessage(source);
  BOOST_CHECK(source_list_observer->m_list.size() == 2);
  source_list_observer->m_list.clear();
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
