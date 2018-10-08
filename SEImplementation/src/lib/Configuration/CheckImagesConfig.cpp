/*
 * CheckImagesConfig.cpp
 *
 *  Created on: May 29, 2017
 *      Author: mschefer
 */

#include <string>
#include <boost/program_options.hpp>

#include "SEImplementation/Configuration/CheckImagesConfig.h"
#include "SEFramework/Image/FitsReader.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string CHECK_MODEL_FITTING { "checkimage-modelfitting" };
static const std::string CHECK_RESIDUAL { "checkimage-residual" };
static const std::string CHECK_MODEL_BACKGROUND { "checkimage-background" };
static const std::string CHECK_MODEL_VARIANCE { "checkimage-variance" };
static const std::string CHECK_SEGMENTATION { "checkimage-segmentation" };
static const std::string CHECK_PARTITION { "checkimage-partition" };
static const std::string CHECK_FILTERED { "checkimage-filtered" };
static const std::string CHECK_AUTO_APERTURE { "checkimage-auto-aperture" };
static const std::string CHECK_APERTURE { "checkimage-aperture" };

CheckImagesConfig::CheckImagesConfig(long manager_id) :
    Configuration(manager_id) {}

std::map<std::string, Configuration::OptionDescriptionList> CheckImagesConfig::getProgramOptions() {
  return { {"Check images", {
      {CHECK_MODEL_FITTING.c_str(), po::value<std::string>()->default_value(""),
          "Path to save the model fitting check image"},
      {CHECK_RESIDUAL.c_str(), po::value<std::string>()->default_value(""),
          "Path to save the model fitting residual check image"},
      {CHECK_MODEL_BACKGROUND.c_str(), po::value<std::string>()->default_value(""),
        "Path to save the background check image"},
      {CHECK_MODEL_VARIANCE.c_str(), po::value<std::string>()->default_value(""),
        "Path to save the variance check image"},
      {CHECK_SEGMENTATION.c_str(), po::value<std::string>()->default_value(""),
        "Path to save the segmentation check image"},
      {CHECK_PARTITION.c_str(), po::value<std::string>()->default_value(""),
        "Path to save the partition check image"},
      {CHECK_FILTERED.c_str(), po::value<std::string>()->default_value(""),
        "Path to save the filtered check image"},
      {CHECK_AUTO_APERTURE.c_str(), po::value<std::string>()->default_value(""),
        "Path to save the auto aperture check image"},
      {CHECK_APERTURE.c_str(), po::value<std::string>()->default_value(""),
        "Path to save the aperture check image"},
  }}};
}

void CheckImagesConfig::initialize(const UserValues& args) {
  m_model_fitting_filename = args.find(CHECK_MODEL_FITTING)->second.as<std::string>();
  m_model_fitting_residual_filename = args.find(CHECK_RESIDUAL)->second.as<std::string>();
  m_model_background_filename = args.find(CHECK_MODEL_BACKGROUND)->second.as<std::string>();
  m_model_variance_filename = args.find(CHECK_MODEL_VARIANCE)->second.as<std::string>();
  m_segmentation_filename = args.find(CHECK_SEGMENTATION)->second.as<std::string>();
  m_partition_filename = args.find(CHECK_PARTITION)->second.as<std::string>();
  m_filtered_filename = args.find(CHECK_FILTERED)->second.as<std::string>();
  m_auto_aperture_filename = args.find(CHECK_AUTO_APERTURE)->second.as<std::string>();
  m_aperture_filename = args.find(CHECK_APERTURE)->second.as<std::string>();
}

} // SExtractor namespace


