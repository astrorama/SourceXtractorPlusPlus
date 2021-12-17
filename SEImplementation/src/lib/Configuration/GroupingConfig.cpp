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
/*
 * GroupingConfig.cpp
 *
 *  Created on: Jul 3, 2018
 *      Author: mschefer
 */

#include <boost/algorithm/string.hpp>

#include "ElementsKernel/Exception.h"
#include "Configuration/ConfigManager.h"

#include "SEImplementation/Configuration/GroupingConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

static const std::string GROUPING_ALGORITHM {"grouping-algorithm" };
static const std::string GROUPING_HARD_LIMIT {"grouping-hard-limit" };
static const std::string GROUPING_MOFFAT_THRESHOLD {"grouping-moffat-threshold" };
static const std::string GROUPING_MOFFAT_MAX_DISTANCE {"grouping-moffat-max-distance" };

static const std::string GROUPING_ALGORITHM_NONE {"NONE" };
static const std::string GROUPING_ALGORITHM_OVERLAP {"OVERLAP" };
static const std::string GROUPING_ALGORITHM_SPLIT {"SPLIT" };
static const std::string GROUPING_ALGORITHM_MOFFAT {"MOFFAT" };

GroupingConfig::GroupingConfig(long manager_id)
    : Configuration(manager_id),
      m_selected_algorithm(Algorithm::SPLIT_SOURCES), m_moffat_threshold(0.02), m_moffat_max_distance(300), m_hard_limit(0) {
}

std::map<std::string, Configuration::OptionDescriptionList> GroupingConfig::getProgramOptions() {
  return { {"Grouping", {
      {GROUPING_ALGORITHM.c_str(), po::value<std::string>()->default_value(GROUPING_ALGORITHM_SPLIT),
          "Grouping algorithm to be used [none|overlap|split|moffat]."},
      {GROUPING_HARD_LIMIT.c_str(), po::value<unsigned int>()->default_value(0),
          "Maximum number of sources in a single group (0 = unlimited)"},
      {GROUPING_MOFFAT_THRESHOLD.c_str(), po::value<double>()->default_value(0.02),
          "Threshold used for Moffat grouping."},
      {GROUPING_MOFFAT_MAX_DISTANCE.c_str(), po::value<double>()->default_value(300),
          "Maximum distance (in pixels) to be considered for grouping"},
  }}};
}

void GroupingConfig::initialize(const UserValues& args) {
  auto algorithm_name = boost::to_upper_copy(args.at(GROUPING_ALGORITHM).as<std::string>());
  if (algorithm_name == GROUPING_ALGORITHM_NONE) {
    m_selected_algorithm = Algorithm::NO_GROUPING;
  } else if (algorithm_name == GROUPING_ALGORITHM_OVERLAP) {
    m_selected_algorithm = Algorithm::OVERLAPPING;
  } else if (algorithm_name == GROUPING_ALGORITHM_SPLIT) {
    m_selected_algorithm = Algorithm::SPLIT_SOURCES;
  } else if (algorithm_name == GROUPING_ALGORITHM_MOFFAT) {
    m_selected_algorithm = Algorithm::MOFFAT;
  } else {
    throw Elements::Exception() << "Unknown grouping algorithm : " << algorithm_name;
  }

  if (args.find(GROUPING_MOFFAT_THRESHOLD) != args.end()) {
    m_moffat_threshold = args.find(GROUPING_MOFFAT_THRESHOLD)->second.as<double>();
  }
  if (args.find(GROUPING_MOFFAT_MAX_DISTANCE) != args.end()) {
    m_moffat_max_distance = args.find(GROUPING_MOFFAT_MAX_DISTANCE)->second.as<double>();
  }
  if (args.find(GROUPING_HARD_LIMIT) != args.end()) {
    m_hard_limit = args.find(GROUPING_HARD_LIMIT)->second.as<unsigned int>();
  }
}

} // SourceXtractor namespace



