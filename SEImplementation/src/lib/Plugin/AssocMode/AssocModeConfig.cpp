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

#include <map>
#include <boost/algorithm/string.hpp>

#include <CCfits/CCfits>

#include "ElementsKernel/Logging.h"

#include "Table/AsciiReader.h"
#include "Table/FitsReader.h"
#include "Table/CastVisitor.h"

#include "SEImplementation/Configuration/DetectionImageConfig.h"
#include "SEImplementation/Configuration/PartitionStepConfig.h"
#include "SEImplementation/Configuration/MultiThresholdPartitionConfig.h"

#include "SEImplementation/Plugin/AssocMode/AssocModePartitionStep.h"

#include "SEImplementation/Plugin/AssocMode/AssocModeConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

static Elements::Logging logger = Elements::Logging::getLogger("AssocModeConfig");

enum class AssocCoordType {
  PIXEL,
  WORLD
};

static const std::string ASSOC_CATALOG { "assoc-catalog" };
static const std::string ASSOC_MODE { "assoc-mode" };
static const std::string ASSOC_RADIUS { "assoc-radius" };
static const std::string ASSOC_FILTER { "assoc-filter" };
static const std::string ASSOC_COPY { "assoc-copy" };
static const std::string ASSOC_COLUMNS { "assoc-columns" };
static const std::string ASSOC_COORD_TYPE { "assoc-coord-type" };

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

const std::map<std::string, AssocCoordType> assoc_coord_type_table {
  std::make_pair("PIXEL", AssocCoordType::PIXEL),
  std::make_pair("WORLD", AssocCoordType::WORLD)
};

std::vector<int> parseColumnList(const std::string& arg) {
  if (arg.size() > 0) {
    try {
      std::vector<std::string> parts;
      boost::split(parts, arg, boost::is_any_of(","));

      std::vector<int> column_list;
      for (auto& part : parts) {
        // the input is a 1-based index, the internal index is 0-based
        column_list.emplace_back(boost::lexical_cast<int>(part)-1);
      }
      return column_list;
    } catch(...) {
      throw Elements::Exception() << "Can't parse column list to int: " << arg;
    }
  } else {
    return {};
  }
}

}

AssocModeConfig::AssocModeConfig(long manager_id) : Configuration(manager_id), m_assoc_mode(AssocMode::UNKNOWN), m_assoc_radius(0.) {
  declareDependency<DetectionImageConfig>();
  declareDependency<PartitionStepConfig>();
  ConfigManager::getInstance(manager_id).registerDependency<AssocModeConfig, MultiThresholdPartitionConfig>();
}

std::map<std::string, Configuration::OptionDescriptionList> AssocModeConfig::getProgramOptions() {
  return { {"Assoc config", {
      {ASSOC_CATALOG.c_str(), po::value<std::string>(),
          "Assoc catalog file"},
      {ASSOC_COLUMNS.c_str(), po::value<std::string>()->default_value("1,2"),
          "Assoc columns to specify x/ra,y/dec[,weight] (the index of the first column is 1)"},
      {ASSOC_MODE.c_str(), po::value<std::string>()->default_value("NEAREST"),
          "Assoc mode [FIRST, NEAREST, MEAN, MAG_MEAN, SUM, MAG_SUM, MIN, MAX]"},
      {ASSOC_RADIUS.c_str(), po::value<double>()->default_value(2.0),
          "Assoc radius (Always in pixels of the detection image)"},
      {ASSOC_FILTER.c_str(), po::value<std::string>()->default_value("ALL"),
          "Assoc catalog filter setting: ALL, MATCHED, UNMATCHED"},
      {ASSOC_COPY.c_str(), po::value<std::string>()->default_value(""),
          "List of columns indices in the assoc catalog to copy on match (the index of the first column is 1). "},
      {ASSOC_COORD_TYPE.c_str(), po::value<std::string>()->default_value("PIXEL"),
          "Assoc coordinates type: PIXEL, WORLD"},
  }}};
}

void AssocModeConfig::initialize(const UserValues& args) {
  readConfig(args);
  readCatalogs(args);
}

void AssocModeConfig::readConfig(const UserValues& args) {
  auto filter = boost::to_upper_copy(args.at(ASSOC_FILTER).as<std::string>());
  if (assoc_filter_table.find(filter) != assoc_filter_table.end()) {
    auto assoc_filter = assoc_filter_table.at(filter);
    if (assoc_filter == AssocFilter::MATCHED) {
      getDependency<PartitionStepConfig>().addPartitionStepCreator(
              [](std::shared_ptr<SourceFactory> ) {
                return std::make_shared<AssocModePartitionStep>(true);
              }
      );
    } else if (assoc_filter == AssocFilter::UNMATCHED) {
      getDependency<PartitionStepConfig>().addPartitionStepCreator(
              [](std::shared_ptr<SourceFactory> ) {
                return std::make_shared<AssocModePartitionStep>(false);
              }
      );
    }
  } else {
    throw Elements::Exception() << "Invalid assoc filter: " << filter;
  }

  if (args.find(ASSOC_MODE) != args.end()) {
    auto assoc_mode = boost::to_upper_copy(args.at(ASSOC_MODE).as<std::string>());
    if (assoc_mode_table.find(assoc_mode) != assoc_mode_table.end()) {
      m_assoc_mode = assoc_mode_table.at(assoc_mode);
    } else {
      throw Elements::Exception() << "Invalid association mode: " << assoc_mode;
    }
  }

  m_assoc_radius = args.at(ASSOC_RADIUS).as<double>();
}

void AssocModeConfig::readCatalogs(const UserValues& args) {
  auto columns =  parseColumnList(args.at(ASSOC_COLUMNS).as<std::string>());
  if (columns.size() < 2) {
    throw Elements::Exception() << "At least 2 columns must be specified for x,y coordinates in the assoc catalog";
  }
  if (columns.size() > 3) {
    throw Elements::Exception() << "Maximum 3 columns for x, y and weight must be specified in the assoc catalog";
  }

  m_columns_idx = parseColumnList(args.at(ASSOC_COPY).as<std::string>());

  AssocCoordType assoc_coord_type = AssocCoordType::PIXEL;
  if (args.find(ASSOC_COORD_TYPE) != args.end()) {
    auto assoc_coord_type_str = boost::to_upper_copy(args.at(ASSOC_COORD_TYPE).as<std::string>());
    if (assoc_coord_type_table.find(assoc_coord_type_str) != assoc_coord_type_table.end()) {
      assoc_coord_type = assoc_coord_type_table.at(assoc_coord_type_str);
    } else {
      throw Elements::Exception() << "Invalid association coordinate type: " << assoc_coord_type_str;
    }
  }

  if (args.find(ASSOC_CATALOG) != args.end()) {
    auto filename = args.at(ASSOC_CATALOG).as<std::string>();
    try {
      std::shared_ptr<Euclid::Table::TableReader> reader;
      try {
        reader = std::make_shared<Euclid::Table::FitsReader>(filename);
      } catch (...) {
        // If FITS not successful try reading as ascii
        reader = std::make_shared<Euclid::Table::AsciiReader>(filename);
      }
      auto table = reader->read();

      for (size_t i = 0; i < getDependency<DetectionImageConfig>().getExtensionsNb(); i++) {
        if (assoc_coord_type == AssocCoordType::WORLD) {
          auto coordinate_system = getDependency<DetectionImageConfig>().getCoordinateSystem(i);
          m_catalogs.emplace_back(readTable(table, columns, m_columns_idx, coordinate_system));
        } else {
          m_catalogs.emplace_back(readTable(table, columns, m_columns_idx, nullptr));
        }
      }
    } catch (const std::exception& e) {
      throw Elements::Exception() << "Can't either open or read assoc catalog: " << filename << " (" << e.what() << ")";
    } catch(...) {
      throw Elements::Exception() << "Can't either open or read assoc catalog: " << filename;
    }
  }

  if (assoc_coord_type == AssocCoordType::PIXEL && getDependency<DetectionImageConfig>().getExtensionsNb() > 1) {
    logger.warn() <<
        "Using Assoc catalog matching in pixel coordinates with multiple detection images";
  }
}

std::vector<AssocModeConfig::CatalogEntry> AssocModeConfig::readTable(
    const Euclid::Table::Table& table, const std::vector<int>& columns,
    const std::vector<int>& copy_columns, std::shared_ptr<CoordinateSystem> coordinate_system) {
  using Euclid::Table::CastVisitor;

  std::vector<CatalogEntry> catalog;
  for (auto& row : table) {
    // our internal pixel coordinates are zero-based

    ImageCoordinate coord;
    if (coordinate_system != nullptr) {
      auto world_coord = WorldCoordinate{
          boost::apply_visitor(CastVisitor<double>{}, row[columns.at(0)]),
          boost::apply_visitor(CastVisitor<double>{}, row[columns.at(1)]),
      };

      coord = coordinate_system->worldToImage(world_coord);
    } else {
      coord = ImageCoordinate{
          boost::apply_visitor(CastVisitor<double>{}, row[columns.at(0)]) - 1.0,
          boost::apply_visitor(CastVisitor<double>{}, row[columns.at(1)]) - 1.0,
      };
    }
    catalog.emplace_back(CatalogEntry { coord, 1.0, {} });
    if (columns.size() == 3 && columns.at(2) >= 0) {
      catalog.back().weight = boost::get<double>(row[columns.at(2)]);
    }
    for (auto column : copy_columns) {
      if (column >= static_cast<int>(row.size())) {
        throw Elements::Exception() << "Column index " << column << " is out of bounds";
      }
      if (row[column].type() == typeid(int)) {
        catalog.back().assoc_columns.emplace_back(boost::get<int>(row[column]));
      } else if (row[column].type() == typeid(double)) {
        catalog.back().assoc_columns.emplace_back(boost::get<double>(row[column]));
      } else if (row[column].type() == typeid(int64_t)) {
        catalog.back().assoc_columns.emplace_back(boost::get<int64_t>(row[column]));
      } else if (row[column].type() == typeid(SeFloat)) {
        catalog.back().assoc_columns.emplace_back(boost::get<SeFloat>(row[column]));
      } else{
        throw Elements::Exception() << "Wrong type in assoc column (must be a numeric type)";
      }
    }
  }
  return catalog;
}


}

