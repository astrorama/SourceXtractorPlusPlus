/**
 * @file tests/src/SourceList_test.cpp
 * @date 05/24/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Task/TaskRegistry.h"
#include "SEFramework/Source/Source.h"

#include "SEFramework/Source/SourceList.h"

using namespace SExtractor;

struct SourceListFixture {
  std::shared_ptr<TaskRegistry> task_registry;
  std::shared_ptr<Source> source_a, source_b, source_c;

  SourceListFixture()
    : task_registry(new TaskRegistry()),
      source_a(new Source(std::vector<PixelCoordinate>(), task_registry)),
      source_b(new Source(std::vector<PixelCoordinate>(), task_registry)),
      source_c(new Source(std::vector<PixelCoordinate>(), task_registry)) {
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SourceList_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( getSources_test, SourceListFixture ) {
  auto source_list = SourceList::getFactory()({});

  auto& sources = source_list->getSources();
  BOOST_CHECK(sources.empty());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( addSource_test, SourceListFixture ) {
  auto source_list = SourceList::getFactory()({});

  source_list->addSource(source_a);
  source_list->addSource(source_b);

  BOOST_CHECK(bool(source_list->getSources() == std::list<std::shared_ptr<Source>>({ source_a, source_b })));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( removeSource_test, SourceListFixture ) {
  auto source_list = SourceList::getFactory()({});

  source_list->addSource(source_a);
  source_list->addSource(source_b);
  source_list->addSource(source_c);

  BOOST_CHECK(bool(source_list->getSources() == std::list<std::shared_ptr<Source>>({ source_a, source_b, source_c })));

  source_list->removeSource(source_b);

  BOOST_CHECK(bool(source_list->getSources() == std::list<std::shared_ptr<Source>>({ source_a, source_c })));

  source_list->removeSource(source_a);

  BOOST_CHECK(bool(source_list->getSources() == std::list<std::shared_ptr<Source>>({ source_c })));

  source_list->removeSource(source_c);

  BOOST_CHECK(source_list->getSources().empty());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( merge_test, SourceListFixture ) {
  auto source_list_a = SourceList::getFactory()({});
  auto source_list_b = SourceList::getFactory()({});

  source_list_a->addSource(source_a);
  source_list_a->addSource(source_b);

  BOOST_CHECK(bool(source_list_a->getSources() == std::list<std::shared_ptr<Source>>({ source_a, source_b })));

  source_list_b->addSource(source_c);

  BOOST_CHECK(bool(source_list_b->getSources() == std::list<std::shared_ptr<Source>>({ source_c })));

  source_list_a->merge(*source_list_b);

  BOOST_CHECK(bool(source_list_a->getSources() == std::list<std::shared_ptr<Source>>({ source_a, source_b, source_c })));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


