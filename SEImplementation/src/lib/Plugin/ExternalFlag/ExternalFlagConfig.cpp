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
 * @file src/lib/ExternalFlagConfig.cpp
 * @date 06/15/16
 * @author nikoapos
 */

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "Configuration/ProgramOptionsHelper.h"

#include "SEFramework/FITS/FitsReader.h"

#include "SEImplementation/Plugin/ExternalFlag/ExternalFlagConfig.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using poh = Euclid::Configuration::ProgramOptionsHelper;

namespace SourceXtractor {

namespace {
  
const std::string FLAG_IMAGE {"flag-image"};
const std::string FLAG_TYPE {"flag-type"};

std::map<std::string, ExternalFlagConfig::Type> available_types {
  {"OR", ExternalFlagConfig::Type::OR},
  {"AND", ExternalFlagConfig::Type::AND},
  {"MIN", ExternalFlagConfig::Type::MIN},
  {"MAX", ExternalFlagConfig::Type::MAX},
  {"MOST", ExternalFlagConfig::Type::MOST}
};

}

auto ExternalFlagConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"External flag options", {
      {poh::wildcard(FLAG_IMAGE).c_str(), po::value<std::string>(),
          "The FITS file containing the external flag, several images can be provided, replace * by any identifier (ex. use numbers)"},
      {poh::wildcard(FLAG_TYPE).c_str(), po::value<std::string>(),
          "The combination type of the external flag (OR, AND, MIN, MAX, MOST)"}
  }}};
}


void ExternalFlagConfig::preInitialize(const UserValues& args) {
  for (auto& name : poh::findWildcardNames({FLAG_IMAGE, FLAG_TYPE}, args)) {
    
    // Check that the user gave both the filename and the type
    if (args.count(poh::wildcard(FLAG_IMAGE, name)) == 0) {
      throw Elements::Exception() << "Missing option " << poh::wildcard(FLAG_IMAGE, name);
    }

    std::string type;
    if (args.count(poh::wildcard(FLAG_TYPE, name)) == 0) {
      type = "OR";
    } else {
      type = boost::to_upper_copy(args.at(poh::wildcard(FLAG_TYPE, name)).as<std::string>());
    }
    
    // Check that the file exists
    auto& filename = args.at(poh::wildcard(FLAG_IMAGE, name)).as<std::string>();
    if (!fs::exists(filename)) {
      throw Elements::Exception() << "File " << filename << " does not exist";
    }
    
    // Check that the type is a valid option
    if (available_types.count(type) == 0) {
      throw Elements::Exception() << "Invalid option " << poh::wildcard(FLAG_TYPE, name)
              << " : " << type;
    }
  }
}

void ExternalFlagConfig::initialize(const UserValues& args) {
  for (auto& name : poh::findWildcardNames({FLAG_IMAGE, FLAG_TYPE}, args)) {
    
    auto& filename = args.at(poh::wildcard(FLAG_IMAGE, name)).as<std::string>();
    auto image = FitsReader<std::int64_t>::readFile(filename);
    
    std::string type_str;
    if (args.count(poh::wildcard(FLAG_TYPE, name)) == 0) {
      type_str = "OR";
    } else {
      type_str = boost::to_upper_copy(args.at(poh::wildcard(FLAG_TYPE, name)).as<std::string>());
    }
    Type type = available_types.at(type_str);
    
    m_flag_info_list.emplace_back(name, FlagInfo{std::move(image), type});
  }
}

auto ExternalFlagConfig::getFlagInfoList() const -> const std::vector<std::pair<std::string, FlagInfo>>& {
  return m_flag_info_list;
}

} // SourceXtractor namespace



