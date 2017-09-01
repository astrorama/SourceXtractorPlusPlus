/*
 * PluginConfiguration.cpp
 *
 *  Created on: Jul 27, 2016
 *      Author: mschefer
 */

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/dll/shared_library.hpp>

#include <boost/tokenizer.hpp>

#include "SEFramework/Configuration/PluginConfig.h"
#include "Configuration/ConfigManager.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string PLUGIN_DIRECTORY { "plugin-directory" };
static const std::string PLUGIN_LIST { "plugin-list" };

PluginConfig::PluginConfig(long manager_id)
    : Configuration(manager_id) {
}

std::map<std::string, Configuration::OptionDescriptionList> PluginConfig::getProgramOptions() {
  return { {"Plugin configuration", {
      {PLUGIN_DIRECTORY.c_str(), po::value<std::string>()->default_value(""),
          "Path to a directory that contains the plugins"},
      {PLUGIN_LIST.c_str(), po::value<std::string>()->default_value(""),
          "Comma separated list of plugins to load (without file extension)"}
  }}};
}

void PluginConfig::initialize(const UserValues& args) {
  m_plugin_path = args.find(PLUGIN_DIRECTORY)->second.as<std::string>();

  auto& plugin_list_string = args.at(PLUGIN_LIST).as<std::string>();

  boost::char_separator<char> separator(",");
  boost::tokenizer<boost::char_separator<char>> tok(plugin_list_string, separator);

  std::copy(tok.begin(), tok.end(), back_inserter(m_plugin_list));
}

std::vector<boost::filesystem::path> PluginConfig::getPluginPaths() const {
  std::vector<boost::filesystem::path> plugin_paths;
  auto directory_path = boost::filesystem::path(m_plugin_path);
  if (boost::filesystem::is_directory(directory_path)) {
    for (auto& plugin_name : m_plugin_list) {
      auto full_path = directory_path / boost::filesystem::path(plugin_name);
      full_path += boost::dll::shared_library::suffix();

      if (boost::filesystem::exists(full_path)) {
        plugin_paths.emplace_back(full_path);
      }
    }
  }
  return plugin_paths;
}

}
