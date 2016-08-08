/**
 * @file tests/src/MinAreaPartitionStep_test.cpp
 * @date 05/30/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/Source.h"
#include "SEFramework/Task/TaskProvider.h"

#include "SEFramework/Pipeline/Partition.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Partition/MinAreaPartitionStep.h"

using namespace SExtractor;

struct MinAreaPartitionFixture {
  std::shared_ptr<MinAreaPartitionStep> min_area_step;
  std::shared_ptr<TaskProvider> task_provider;
  std::shared_ptr<Source> source;

  const static int min_pixels = 3;

  MinAreaPartitionFixture()
    : min_area_step(new MinAreaPartitionStep(min_pixels)),
      task_provider(new TaskProvider(nullptr)) {
  }
};

class SourceObserver : public Observer<std::shared_ptr<SourceInterface>> {
public:
  virtual void handleMessage(const std::shared_ptr<SourceInterface>& source) override {
      m_list.push_back(source);
  }

  std::list<std::shared_ptr<SourceInterface>> m_list;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (MinAreaPartitionStep_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( source_filtered_test, MinAreaPartitionFixture ) {
  source.reset(new Source(task_provider));
  source->setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1,3}, {8,4}});
  auto& pixel_list = source->getProperty<PixelCoordinateList>().getCoordinateList();
  BOOST_CHECK(pixel_list == std::vector<PixelCoordinate>( { PixelCoordinate(1, 3), PixelCoordinate(8, 4) } ));

  Partition partition( { min_area_step } );

  auto source_observer = std::make_shared<SourceObserver>();
  partition.addObserver(source_observer);

  partition.handleMessage(source);

  BOOST_CHECK(source_observer->m_list.empty());
}

BOOST_FIXTURE_TEST_CASE( source_ok_test, MinAreaPartitionFixture ) {
  source.reset(new Source(task_provider));
  source->setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1,3}, {8,4}, {1,2}});
  auto& pixel_list = source->getProperty<PixelCoordinateList>().getCoordinateList();
  BOOST_CHECK(pixel_list == std::vector<PixelCoordinate>( { PixelCoordinate(1, 3),
    PixelCoordinate(8, 4), PixelCoordinate(1,2)  } ));

  Partition partition( { min_area_step } );

  auto source_observer = std::make_shared<SourceObserver>();
  partition.addObserver(source_observer);

  partition.handleMessage(source);

  BOOST_CHECK(source_observer->m_list.size() == 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
