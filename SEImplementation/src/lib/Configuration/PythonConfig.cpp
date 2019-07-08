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

namespace SExtractor {

PythonConfig::PythonConfig(long manager_id) : Configuration(manager_id) { }

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
  auto filename = args.find(PYTHON_CONFIG_FILE)->second.as<std::string>();
  if (!filename.empty()) {
    std::vector<std::string> argv;
    if (args.find(PYTHON_ARGV) != args.end()) {
      argv = args.find(PYTHON_ARGV)->second.as<std::vector<std::string>>();
    }
    PythonInterpreter::getSingleton().runFile(filename, argv);
  }
}

PythonInterpreter& PythonConfig::getInterpreter() const {
  return PythonInterpreter::getSingleton();
}

} // end of namespace SExtractor
