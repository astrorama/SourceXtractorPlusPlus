/** Copyright © 2021 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include "SEImplementation/Plugin/AssocMode/AssocModeConfig.h"

#include <map>
#include <boost/algorithm/string.hpp>

#include <CCfits/CCfits>

#include "Table/AsciiReader.h"
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

std::vector<int> parseColumnList(const std::string& arg) {
  if (arg.size() > 0) {
    try {
      std::vector<std::string> parts;
      boost::split(parts, arg, boost::is_any_of(","));

      std::vector<int> column_list;
      for (auto& part : parts) {
        column_list.emplace_back(boost::lexical_cast<int>(part));
      }

      return column_list;
    } catch(...) {
      throw Elements::Exception() << "Can't parse column list";
    }
  } else {
    return {};
  }
}

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
      {ASSOC_COPY.c_str(), po::value<std::string>()->default_value(""),
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

  auto columns =  parseColumnList(args.at(ASSOC_COLUMNS).as<std::string>());
  if (columns.size() < 2) {
    throw Elements::Exception() << "At least 2 columns must be specified for x,y coordinates in the assoc catalog";
  }
  if (columns.size() > 3) {
    throw Elements::Exception() << "Maximum 3 columns for x, y and weight must be specified in the assoc catalog";
  }

  auto copy_columns = parseColumnList(args.at(ASSOC_COPY).as<std::string>());

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

      std::shared_ptr<Euclid::Table::TableReader> reader;
      try {
        reader = std::make_shared<Euclid::Table::FitsReader>(filename);
      } catch(...) {
        // If FITS not successful try reading as ascii
        reader = std::make_shared<Euclid::Table::AsciiReader>(filename);
      }
      auto table = reader->read();
      readTable(table, columns, copy_columns);

    } catch(...) {
      throw Elements::Exception() << "Can't open/read assoc catalog";
    }
  }
}

void AssocModeConfig::readTable(
    const Euclid::Table::Table& table, const std::vector<int>& columns, const std::vector<int>& copy_columns) {
  for (auto& row : table) {
    // our internal pixel coordinates are zero-based
    auto coord =
        ImageCoordinate { boost::get<double>(row[columns.at(0)]) - 1.0, boost::get<double>(row[columns.at(1)]) - 1.0 };
    m_catalog.emplace_back(CatalogEntry { coord, 1.0, {} });
    if (columns.size() == 3 && columns.at(2) >= 0) {
      m_catalog.back().weight = boost::get<double>(row[columns.at(2)]);
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
}


}

