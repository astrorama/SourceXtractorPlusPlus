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
 * MultiThreadingConfig.cpp
 *
 *  Created on: May 22, 2018
 *      Author: mschefer
 */

#include <boost/thread.hpp>

#include "SEImplementation/Configuration/MultiThreadingConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

static const std::string THREADS_NB {"thread-count"};

MultiThreadingConfig::MultiThreadingConfig(long manager_id) : Configuration(manager_id), m_threads_nb(-1) {
}

auto MultiThreadingConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"Multi-threading", {
      {THREADS_NB.c_str(), po::value<int>()->default_value(-1), "Number of worker threads (-1=automatic, 0=disable all multithreading)"}
  }}};
}

void MultiThreadingConfig::initialize(const UserValues& args) {
    m_threads_nb = args.at(THREADS_NB).as<int>();
    if (m_threads_nb == -1) {
      m_threads_nb = boost::thread::hardware_concurrency();
    } else if (m_threads_nb < -1) {
      throw Elements::Exception("Invalid number of threads.");
    }
}

} // SourceXtractor namespace

