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
 * @file tests/src/MinAreaPartitionStep_test.cpp
 * @date 05/30/16
 * @author mschefer
 */

#include <boost/test/unit_test.hpp>

#include "SEFramework/Source/SimpleSource.h"

#include "SEFramework/Pipeline/Partition.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Partition/MinAreaPartitionStep.h"

using namespace SourceXtractor;

struct MinAreaPartitionFixture {
  const static int min_pixels = 3;
  std::unique_ptr<SimpleSource> source {new SimpleSource};
  std::shared_ptr<MinAreaPartitionStep> min_area_step {new MinAreaPartitionStep(min_pixels)};
};

class SourceObserver : public Observer<SourceInterface> {
public:
  virtual void handleMessage(const SourceInterface& source) override {
      m_list.push_back(source.getProperty<PixelCoordinateList>());
  }

  std::list<PixelCoordinateList> m_list;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (MinAreaPartitionStep_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( source_filtered_test, MinAreaPartitionFixture ) {
  source->setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1,3}, {8,4}});

  Partition partition( { min_area_step } );

  auto source_observer = std::make_shared<SourceObserver>();
  partition.addObserver(source_observer);

  partition.receiveSource(std::move(source));

  BOOST_CHECK(source_observer->m_list.empty());
}

BOOST_FIXTURE_TEST_CASE( source_ok_test, MinAreaPartitionFixture ) {
  source->setProperty<PixelCoordinateList>(std::vector<PixelCoordinate>{{1,3}, {8,4}, {1,2}});

  Partition partition( { min_area_step } );

  auto source_observer = std::make_shared<SourceObserver>();
  partition.addObserver(source_observer);

  partition.receiveSource(std::move(source));

  BOOST_CHECK(source_observer->m_list.size() == 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
