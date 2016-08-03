/**
 * @file tests/src/OverlappingBoundariesSourceList_test.cpp
 * @date 06/08/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/Source.h"
#include "SEFramework/Task/TaskRegistry.h"

#include "SEImplementation/Task/PixelBoundariesTaskFactory.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Grouping/OverlappingBoundariesCriteria.h"
#include "SEImplementation/Grouping/OverlappingBoundariesSourceList.h"

using namespace SExtractor;

struct OverlappingBoundariesSourceListFixture {
  std::shared_ptr<TaskRegistry> task_registry;
  std::shared_ptr<Source> source_a, source_b, source_c;
  std::shared_ptr<OverlappingBoundariesSourceList> source_list;

  OverlappingBoundariesSourceListFixture()
      : task_registry(new TaskRegistry), source_list(new OverlappingBoundariesSourceList) {
    task_registry->registerTaskFactory(std::unique_ptr<TaskFactory>(new PixelBoundariesTaskFactory));
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (OverlappingBoundariesSourceList_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( OverlappingBoundariesSourceListTest, OverlappingBoundariesSourceListFixture ) {
  source_a.reset(new Source(task_registry));
  source_a->setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1,3}, {2,4}});
  source_b.reset(new Source(task_registry));
  source_b->setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1,4}, {1,5}});
  source_c.reset(new Source(task_registry));
  source_c->setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1,5}, {1,6}});

  source_list->addSource(source_a);
  source_list->addSource(source_b);
  source_list->addSource(source_c);

  BOOST_CHECK(source_list->getMin() == PixelCoordinate(1,3));
  BOOST_CHECK(source_list->getMax() == PixelCoordinate(2,6));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
