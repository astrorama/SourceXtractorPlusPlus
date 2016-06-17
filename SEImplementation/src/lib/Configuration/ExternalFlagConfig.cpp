/**
 * @file src/lib/ExternalFlagConfig.cpp
 * @date 06/15/16
 * @author nikoapos
 */

#include <boost/filesystem.hpp>
#include "Configuration/ProgramOptionsHelper.h"
#include "SEFramework/Image/FitsReader.h"
#include "SEImplementation/Configuration/ExternalFlagConfig.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using poh = Euclid::Configuration::ProgramOptionsHelper;

namespace SExtractor {

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
          "The FITS file containing the external flag"},
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
    if (args.count(poh::wildcard(FLAG_TYPE, name)) == 0) {
      throw Elements::Exception() << "Missing option " << poh::wildcard(FLAG_TYPE, name);
    }
    
    // Check that the file exists
    auto& filename = args.at(poh::wildcard(FLAG_IMAGE, name)).as<std::string>();
    if (!fs::exists(filename)) {
      throw Elements::Exception() << "File " << filename << " does not exist";
    }
    
    // Check that the type is a valid option
    auto& type = args.at(poh::wildcard(FLAG_TYPE, name)).as<std::string>();
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
    
    auto& type_str = args.at(poh::wildcard(FLAG_TYPE, name)).as<std::string>();
    Type type = available_types.at(type_str);
    
    m_flag_info_list.emplace_back(name, FlagInfo{std::move(image), type});
  }
}

auto ExternalFlagConfig::getFlagInfoList() const -> const std::vector<std::pair<std::string, FlagInfo>>& {
  return m_flag_info_list;
}

} // SExtractor namespace



