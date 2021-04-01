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

#include <string>
#include <boost/test/unit_test.hpp>
#include <boost/random.hpp>

#include "SEFramework/Source/SimpleSource.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/AssocMode/AssocMode.h"
#include "SEImplementation/Plugin/AssocMode/AssocModeTask.h"

using namespace SourceXtractor;

struct AssocModeFixture {
  SimpleSource source;
  std::vector<AssocModeConfig::CatalogEntry> catalog {
    { {110, 100}, 1.0, {2.0, 3.0} },
    { {50, 50}, 1.0, {2.0, 3.0} },
    { {50, 60}, 0.5, {4.0, 5.0} },
    { {60, 60}, 3.0, {6.0, 7.0} },
    { {60, 50}, 2.0, {8.0, 9.0} }
  };

  AssocModeFixture() {}
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(AssocMode_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(CheckAssocFail, AssocModeFixture) {
  source.setProperty<PixelCentroid>(100, 100);

  AssocModeTask assoc_mode_task(catalog, AssocModeConfig::AssocMode::FIRST, 5.0);
  assoc_mode_task.computeProperties(source);

  auto assoc_mode_property = source.getProperty<AssocMode>();

  BOOST_CHECK(!assoc_mode_property.getMatch());
}

BOOST_FIXTURE_TEST_CASE(CheckAssocMatch, AssocModeFixture) {
  source.setProperty<PixelCentroid>(100, 100);

  AssocModeTask assoc_mode_task(catalog, AssocModeConfig::AssocMode::FIRST, 15.0);
  assoc_mode_task.computeProperties(source);

  auto assoc_mode_property = source.getProperty<AssocMode>();

  BOOST_CHECK(assoc_mode_property.getMatch());
}

BOOST_FIXTURE_TEST_CASE(CheckAssocSum, AssocModeFixture) {
  source.setProperty<PixelCentroid>(55, 55);

  AssocModeTask assoc_mode_task(catalog, AssocModeConfig::AssocMode::SUM, 15.0);
  assoc_mode_task.computeProperties(source);

  auto assoc_mode_property = source.getProperty<AssocMode>();

  BOOST_CHECK(assoc_mode_property.getMatch());
  BOOST_CHECK_CLOSE(assoc_mode_property.getAssocValues().at(0), 20.0, 0.001);
  BOOST_CHECK_CLOSE(assoc_mode_property.getAssocValues().at(1), 24.0, 0.001);
}

BOOST_FIXTURE_TEST_CASE(CheckAssocMean, AssocModeFixture) {
  source.setProperty<PixelCentroid>(55, 55);

  AssocModeTask assoc_mode_task(catalog, AssocModeConfig::AssocMode::MEAN, 15.0);
  assoc_mode_task.computeProperties(source);

  auto assoc_mode_property = source.getProperty<AssocMode>();

  BOOST_CHECK(assoc_mode_property.getMatch());
  BOOST_CHECK_CLOSE(assoc_mode_property.getAssocValues().at(0), 38.0 / 6.5, 0.001);
}

BOOST_FIXTURE_TEST_CASE(CheckAssocMagSum, AssocModeFixture) {
  source.setProperty<PixelCentroid>(55, 55);

  AssocModeTask assoc_mode_task(catalog, AssocModeConfig::AssocMode::MAG_SUM, 15.0);
  assoc_mode_task.computeProperties(source);

  auto assoc_mode_property = source.getProperty<AssocMode>();

  BOOST_CHECK(assoc_mode_property.getMatch());
  BOOST_CHECK_CLOSE(assoc_mode_property.getAssocValues().at(0), 1.8133, 0.1);
}

BOOST_FIXTURE_TEST_CASE(CheckAssocMagMean, AssocModeFixture) {
  source.setProperty<PixelCentroid>(55, 55);

  AssocModeTask assoc_mode_task(catalog, AssocModeConfig::AssocMode::MAG_MEAN, 15.0);
  assoc_mode_task.computeProperties(source);

  auto assoc_mode_property = source.getProperty<AssocMode>();

  BOOST_CHECK(assoc_mode_property.getMatch());
  BOOST_CHECK_CLOSE(assoc_mode_property.getAssocValues().at(0), 3.97116, 0.1);
}

BOOST_FIXTURE_TEST_CASE(CheckAssocNearest, AssocModeFixture) {
  source.setProperty<PixelCentroid>(56, 56);

  AssocModeTask assoc_mode_task(catalog, AssocModeConfig::AssocMode::NEAREST, 15.0);
  assoc_mode_task.computeProperties(source);

  auto assoc_mode_property = source.getProperty<AssocMode>();

  BOOST_CHECK(assoc_mode_property.getMatch());
  BOOST_CHECK_CLOSE(assoc_mode_property.getAssocValues().at(0), 6.0, 0.001);
  BOOST_CHECK_CLOSE(assoc_mode_property.getAssocValues().at(1), 7.0, 0.001);
}

BOOST_FIXTURE_TEST_CASE(CheckAssocMin, AssocModeFixture) {
  source.setProperty<PixelCentroid>(55, 55);

  AssocModeTask assoc_mode_task(catalog, AssocModeConfig::AssocMode::MIN, 15.0);
  assoc_mode_task.computeProperties(source);

  auto assoc_mode_property = source.getProperty<AssocMode>();

  BOOST_CHECK(assoc_mode_property.getMatch());
  BOOST_CHECK_CLOSE(assoc_mode_property.getAssocValues().at(0), 4.0, 0.001);
  BOOST_CHECK_CLOSE(assoc_mode_property.getAssocValues().at(1), 5.0, 0.001);
}

BOOST_FIXTURE_TEST_CASE(CheckAssocMax, AssocModeFixture) {
  source.setProperty<PixelCentroid>(55, 55);

  AssocModeTask assoc_mode_task(catalog, AssocModeConfig::AssocMode::MAX, 15.0);
  assoc_mode_task.computeProperties(source);

  auto assoc_mode_property = source.getProperty<AssocMode>();

  BOOST_CHECK(assoc_mode_property.getMatch());
  BOOST_CHECK_CLOSE(assoc_mode_property.getAssocValues().at(0), 6.0, 0.001);
  BOOST_CHECK_CLOSE(assoc_mode_property.getAssocValues().at(1), 7.0, 0.001);
}

BOOST_FIXTURE_TEST_CASE(CheckLargeCatalog, AssocModeFixture) {
  boost::random::mt19937 rng { (unsigned int) time(NULL) } ;
  std::vector<AssocModeConfig::CatalogEntry> large_catalog;

  for (int i=0; i<10000; i++) {
    large_catalog.emplace_back(AssocModeConfig::CatalogEntry { {boost::random::uniform_real_distribution<>(-100.0, 100.0)(rng),
      boost::random::uniform_real_distribution<>(-100.0, 100.0)(rng)}, 1.0, {1.0}});
  }
  BOOST_CHECK_EQUAL(large_catalog.size(), 10000);

  for (int i=0; i<15000; i++) {
    large_catalog.emplace_back(AssocModeConfig::CatalogEntry { {boost::random::uniform_real_distribution<>(-100.0, 100.0)(rng),
      boost::random::uniform_real_distribution<>(-100.0, 100.0)(rng) + 1000}, 1.0, {1.0}});
  }
  BOOST_CHECK_EQUAL(large_catalog.size(), 25000);

  large_catalog.emplace_back(AssocModeConfig::CatalogEntry { { 55.0, 55.0 }, 1.0, {0.0}});
  BOOST_CHECK_EQUAL(large_catalog.size(), 25001);


  {
    source.setProperty<PixelCentroid>(0, 0);
    AssocModeTask assoc_mode_task(large_catalog, AssocModeConfig::AssocMode::SUM, 150.0);
    assoc_mode_task.computeProperties(source);

    auto assoc_mode_property = source.getProperty<AssocMode>();
    BOOST_CHECK(assoc_mode_property.getMatch());
    BOOST_CHECK_CLOSE(assoc_mode_property.getAssocValues().at(0), 10000.0, 0.001);
  }
  {
    source.setProperty<PixelCentroid>(0, 1000);
    AssocModeTask assoc_mode_task(large_catalog, AssocModeConfig::AssocMode::SUM, 150.0);
    assoc_mode_task.computeProperties(source);

    auto assoc_mode_property = source.getProperty<AssocMode>();
    BOOST_CHECK(assoc_mode_property.getMatch());
    BOOST_CHECK_CLOSE(assoc_mode_property.getAssocValues().at(0), 15000.0, 0.001);
  }
  {
    source.setProperty<PixelCentroid>(0, 0);
    AssocModeTask assoc_mode_task(large_catalog, AssocModeConfig::AssocMode::SUM, 5000.0);
    assoc_mode_task.computeProperties(source);

    auto assoc_mode_property = source.getProperty<AssocMode>();
    BOOST_CHECK(assoc_mode_property.getMatch());
    BOOST_CHECK_CLOSE(assoc_mode_property.getAssocValues().at(0), 25000.0, 0.001);
  }
  {
    source.setProperty<PixelCentroid>(55.0, 55.0);
    AssocModeTask assoc_mode_task(large_catalog, AssocModeConfig::AssocMode::NEAREST, 20.0);
    assoc_mode_task.computeProperties(source);

    auto assoc_mode_property = source.getProperty<AssocMode>();
    BOOST_CHECK(assoc_mode_property.getMatch());
    BOOST_CHECK_CLOSE(assoc_mode_property.getAssocValues().at(0), 0.0, 0.001);
  }
}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

