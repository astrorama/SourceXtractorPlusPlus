/*
 * CheckImagesConfig.cpp
 *
 *  Created on: May 29, 2017
 *      Author: mschefer
 */

#include "SEImplementation/Configuration/CheckImagesConfig.h"
#include "SEFramework/Image/FitsReader.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string CHECK_MODEL_FITTING { "checkimage-modelfitting" };
static const std::string CHECK_RESIDUAL { "checkimage-residual" };
static const std::string CHECK_MODEL_BACKGROUND { "checkimage-background" };

CheckImagesConfig::CheckImagesConfig(long manager_id) :
    Configuration(manager_id) {}

std::map<std::string, Configuration::OptionDescriptionList> CheckImagesConfig::getProgramOptions() {
  return { {"Check images", {
      {CHECK_MODEL_FITTING.c_str(), po::value<std::string>()->default_value(""),
          "Path to save the model fitting check image"},
      {CHECK_RESIDUAL.c_str(), po::value<std::string>()->default_value(""),
          "Path to save the model fitting residual check image"},
      {CHECK_MODEL_BACKGROUND.c_str(), po::value<std::string>()->default_value(""),
        "Path to save the background check image"}  }}};
}

void CheckImagesConfig::initialize(const UserValues& args) {
  m_model_fitting_filename = args.find(CHECK_MODEL_FITTING)->second.as<string>();
  m_model_fitting_residual_filename = args.find(CHECK_RESIDUAL)->second.as<string>();
  m_model_background_filename = args.find(CHECK_MODEL_BACKGROUND)->second.as<string>();
}

} // SExtractor namespace


