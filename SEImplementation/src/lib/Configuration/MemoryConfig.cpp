/*
 * MemoryConfig.cpp
 *
 *  Created on: Mar 21, 2018
 *      Author: mschefer
 */

#include "SEImplementation/Configuration/MemoryConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string MAX_TILE_MEMORY {"max-tile-memory"};
static const std::string TILE_SIZE {"tile-size"};

MemoryConfig::MemoryConfig(long manager_id) : Configuration(manager_id) {
}

auto MemoryConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"Memory usage", {
      {MAX_TILE_MEMORY.c_str(), po::value<int>()->default_value(512), "Maximum memory used for image tiles cache in megabytes"},
      {TILE_SIZE.c_str(), po::value<int>()->default_value(256), "Image tiles size in pixels"},
  }}};

  return {};
}

void MemoryConfig::initialize(const UserValues& args) {
  m_max_memory = args.at(MAX_TILE_MEMORY).as<int>();
  m_tile_size = args.at(TILE_SIZE).as<int>();
  if (m_max_memory <= 0) {
    throw Elements::Exception() << "Invalid " << MAX_TILE_MEMORY << " value: " << m_max_memory;
  }
  if (m_tile_size <= 0) {
    throw Elements::Exception() << "Invalid " << TILE_SIZE << " value: " << m_tile_size;
  }
}

} /* namespace SExtractor */




