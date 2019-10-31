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
 * @file PythonConfig.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <boost/filesystem.hpp>
#include <SEImplementation/Configuration/PythonConfig.h>

using namespace Euclid::Configuration;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace {

const std::string PYTHON_CONFIG_FILE { "python-config-file" };
const std::string PYTHON_ARGV { "python-arg" };

}

namespace SourceXtractor {

PythonConfig::PythonConfig(long manager_id) : Configuration(manager_id) {
  PythonInterpreter::getSingleton();
}

std::map<std::string, Configuration::OptionDescriptionList> PythonConfig::getProgramOptions() {
  return {{"Measurement config", {
    {PYTHON_CONFIG_FILE.c_str(), po::value<std::string>()->default_value({}, ""),
        "Measurements python configuration file"},
    {PYTHON_ARGV.c_str(), po::value<std::vector<std::string>>()->multitoken(),
         "Parameters to pass to Python via sys.argv"}
  }}};
}


void PythonConfig::preInitialize(const UserValues& args) {
  auto filename = args.find(PYTHON_CONFIG_FILE)->second.as<std::string>();
  if (!filename.empty() && !fs::exists(filename)) {
    throw Elements::Exception() << "Python configuration file " << filename
        << " does not exist";
  }
}

void PythonConfig::initialize(const UserValues& args) {
  auto &singleton = PythonInterpreter::getSingleton();
  auto filename = args.find(PYTHON_CONFIG_FILE)->second.as<std::string>();
  if (!filename.empty()) {
    std::vector<std::string> argv;
    if (args.find(PYTHON_ARGV) != args.end()) {
      argv = args.find(PYTHON_ARGV)->second.as<std::vector<std::string>>();
    }
    singleton.runFile(filename, argv);
  }
}

PythonInterpreter& PythonConfig::getInterpreter() const {
  return PythonInterpreter::getSingleton();
}

} // end of namespace SourceXtractor
