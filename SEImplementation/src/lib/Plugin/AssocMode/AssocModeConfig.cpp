/*
 * AssocConfig.cpp
 *
 *  Created on: Mar 4, 2021
 *      Author: mschefer
 */

#include "SEImplementation/Plugin/AssocMode/AssocModeConfig.h"

#include <map>
#include <boost/algorithm/string.hpp>

#include <CCfits/CCfits>

#include "Table/FitsReader.h"

#include "SEImplementation/Configuration/PartitionStepConfig.h"
#include "SEImplementation/Configuration/MultiThresholdPartitionConfig.h"

#include "SEImplementation/Plugin/AssocMode/AssocModePartitionStep.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

static const std::string ASSOC_CATALOG { "assoc-catalog" };
static const std::string ASSOC_MODE { "assoc-mode" };
static const std::string ASSOC_RADIUS { "assoc-radius" };
static const std::string ASSOC_FILTER { "assoc-filter" };
static const std::string ASSOC_COPY { "assoc-copy" };
static const std::string ASSOC_COLUMNS { "assoc-columns" };

namespace {

const std::map<std::string, AssocModeConfig::AssocMode> assoc_mode_table {
  std::make_pair("", AssocModeConfig::AssocMode::UNKNOWN),
  std::make_pair("FIRST", AssocModeConfig::AssocMode::FIRST),
  std::make_pair("NEAREST", AssocModeConfig::AssocMode::NEAREST),
  std::make_pair("MEAN", AssocModeConfig::AssocMode::MEAN),
  std::make_pair("MAG_MEAN", AssocModeConfig::AssocMode::MAG_MEAN),
  std::make_pair("SUM", AssocModeConfig::AssocMode::SUM),
  std::make_pair("MAG_SUM", AssocModeConfig::AssocMode::MAG_SUM),
  std::make_pair("MIN", AssocModeConfig::AssocMode::MIN),
  std::make_pair("MAX", AssocModeConfig::AssocMode::MAX)
};

const std::map<std::string, AssocModeConfig::AssocFilter> assoc_filter_table {
  std::make_pair("ALL", AssocModeConfig::AssocFilter::ALL),
  std::make_pair("MATCHED", AssocModeConfig::AssocFilter::MATCHED),
  std::make_pair("UNMATCHED", AssocModeConfig::AssocFilter::UNMATCHED)
};

}

AssocModeConfig::AssocModeConfig(long manager_id) :
    Configuration(manager_id) {
  declareDependency<PartitionStepConfig>();
  ConfigManager::getInstance(manager_id).registerDependency<AssocModeConfig, MultiThresholdPartitionConfig>();
}

std::map<std::string, Configuration::OptionDescriptionList> AssocModeConfig::getProgramOptions() {
  return { {"Assoc config", {
      {ASSOC_CATALOG.c_str(), po::value<std::string>(),
          "Assoc catalog file"},
      {ASSOC_COLUMNS.c_str(), po::value<std::string>()->default_value("2,3,4"),
          "Assoc columns"},
      {ASSOC_MODE.c_str(), po::value<std::string>()->default_value("NEAREST"),
          "Assoc mode"},
      {ASSOC_RADIUS.c_str(), po::value<double>()->default_value(2.0),
          "Assoc radius"},
      {ASSOC_FILTER.c_str(), po::value<std::string>()->default_value("ALL"),
          "Assoc catalog filter setting: ALL, MATCHED, UNMATCHED"},
      {ASSOC_COPY.c_str(), po::value<std::string>(),
          "List of assoc catalog columns to copy on match"},
  }}};
}

void AssocModeConfig::initialize(const UserValues& args) {

  auto filter = boost::to_upper_copy(args.at(ASSOC_FILTER).as<std::string>());
  if (assoc_filter_table.find(filter) != assoc_filter_table.end()) {
      auto assoc_filter = assoc_filter_table.at(filter);
      if (assoc_filter == AssocFilter::MATCHED) {
        getDependency<PartitionStepConfig>().addPartitionStepCreator(
                [](std::shared_ptr<SourceFactory> source_factory) {
                  return std::make_shared<AssocModePartitionStep>(true);
                }
        );
      } else if (assoc_filter == AssocFilter::UNMATCHED) {
        getDependency<PartitionStepConfig>().addPartitionStepCreator(
                [](std::shared_ptr<SourceFactory> source_factory) {
                  return std::make_shared<AssocModePartitionStep>(false);
                }
        );
      }
  } else {
      throw Elements::Exception() << "Invalid assoc filter: " << filter;
  }

  m_assoc_radius = args.at(ASSOC_RADIUS).as<double>();

  int columns[3] = {4, 5, -1}; // FIXME tmp
  std::vector<int> copy_columns {2, 3}; // FIXME tmp

  if (args.find(ASSOC_MODE) != args.end()) {
    auto assoc_mode = boost::to_upper_copy(args.at(ASSOC_MODE).as<std::string>());
    if (assoc_mode_table.find(assoc_mode) != assoc_mode_table.end()) {
      m_assoc_mode = assoc_mode_table.at(assoc_mode);
    } else {
      throw Elements::Exception() << "Invalid association mode: " << assoc_mode;
    }
  }

  if (args.find(ASSOC_CATALOG) != args.end()) {
    try {
      auto filename = args.at(ASSOC_CATALOG).as<std::string>();
      Euclid::Table::FitsReader reader(filename);
      auto table = reader.read();
      //auto& column_info = reader.getInfo();

      for (auto& row : table) {
        auto coord = ImageCoordinate { boost::get<double>(row[columns[0]]), boost::get<double>(row[columns[1]]) };
        m_catalog.emplace_back(CatalogEntry { coord, 1.0, {} });
        if (columns[2] >= 0) {
          m_catalog.back().weight = boost::get<double>(row[columns[2]]);
        }
        for (auto column : copy_columns) {
          if (row[column].type() == typeid(int)) {
            m_catalog.back().assoc_columns.emplace_back(boost::get<int>(row[column]));
          } else if (row[column].type() == typeid(double)) {
            m_catalog.back().assoc_columns.emplace_back(boost::get<double>(row[column]));
          } else {
            throw Elements::Exception() << "Wrong type in assoc column";
          }
        }
      }
    } catch(...) {
      throw Elements::Exception() << "Can't open assoc catalog mode";
    }
  }
}

}

