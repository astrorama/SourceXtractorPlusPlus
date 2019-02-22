/*
 * MultiThreadingConfig.cpp
 *
 *  Created on: May 22, 2018
 *      Author: mschefer
 */

#include "SEImplementation/Configuration/MultiThreadingConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string THREADS_NB {"threads-nb"};

MultiThreadingConfig::MultiThreadingConfig(long manager_id) : Configuration(manager_id) {
}

auto MultiThreadingConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"Multi-threading", {
      {THREADS_NB.c_str(), po::value<int>()->default_value(4), "Number of worker threads (0=disable all multithreading)"}
  }}};
}

void MultiThreadingConfig::initialize(const UserValues& args) {
    m_threads_nb = args.at(THREADS_NB).as<int>();
    if (m_threads_nb < 0) {
      throw Elements::Exception("The number of threads can not be negative!");
    }
}

} // SExtractor namespace

