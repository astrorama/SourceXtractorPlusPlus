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
 * MemoryConfig.cpp
 *
 *  Created on: Mar 21, 2018
 *      Author: mschefer
 */

#include "SEImplementation/Configuration/MemoryConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

static const std::string MAX_TILE_MEMORY {"tile-memory-limit"};
static const std::string TILE_SIZE {"tile-size"};

MemoryConfig::MemoryConfig(long manager_id) : Configuration(manager_id), m_max_memory(512), m_tile_size(256) {
}

auto MemoryConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"Memory usage", {
      {MAX_TILE_MEMORY.c_str(), po::value<int>()->default_value(512), "Maximum memory used for image tiles cache in megabytes"},
      {TILE_SIZE.c_str(), po::value<int>()->default_value(256), "Image tiles size in pixels"},
  }}};
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

} /* namespace SourceXtractor */




