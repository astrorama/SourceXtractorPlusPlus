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
 * CheckImagesConfig.cpp
 *
 *  Created on: May 29, 2017
 *      Author: mschefer
 */

#include <string>
#include <boost/program_options.hpp>

#include "SEImplementation/Configuration/CheckImagesConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string CHECK_MODEL_FITTING { "check-image-model-fitting" };
static const std::string CHECK_RESIDUAL { "check-image-residual" };
static const std::string CHECK_MODEL_BACKGROUND { "check-image-background" };
static const std::string CHECK_MODEL_VARIANCE { "check-image-variance" };
static const std::string CHECK_SEGMENTATION { "check-image-segmentation" };
static const std::string CHECK_PARTITION { "check-image-partition" };
static const std::string CHECK_GROUPING { "check-image-grouping" };
static const std::string CHECK_FILTERED { "check-image-filtered" };
static const std::string CHECK_THRESHOLDED { "check-image-thresholded" };
static const std::string CHECK_AUTO_APERTURE { "check-image-auto-aperture" };
static const std::string CHECK_APERTURE { "check-image-aperture" };
static const std::string CHECK_MOFFAT { "check-image-moffat" };
static const std::string CHECK_PSF { "check-image-psf" };

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
      {CHECK_GROUPING.c_str(), po::value<std::string>()->default_value(""),
        "Path to save the grouping check image"},
      {CHECK_FILTERED.c_str(), po::value<std::string>()->default_value(""),
        "Path to save the filtered check image"},
      {CHECK_THRESHOLDED.c_str(), po::value<std::string>()->default_value(""),
        "Path to save the thresholded check image"},
      {CHECK_AUTO_APERTURE.c_str(), po::value<std::string>()->default_value(""),
        "Path to save the auto aperture check image"},
      {CHECK_APERTURE.c_str(), po::value<std::string>()->default_value(""),
        "Path to save the aperture check image"},
      {CHECK_MOFFAT.c_str(), po::value<std::string>()->default_value(""),
        "Path to save the moffat check image"},
      {CHECK_PSF.c_str(), po::value<std::string>()->default_value(""),
        "Path to save the PSF check image"},
  }}};
}

void CheckImagesConfig::initialize(const UserValues& args) {
  m_model_fitting_filename = args.find(CHECK_MODEL_FITTING)->second.as<std::string>();
  m_model_fitting_residual_filename = args.find(CHECK_RESIDUAL)->second.as<std::string>();
  m_model_background_filename = args.find(CHECK_MODEL_BACKGROUND)->second.as<std::string>();
  m_model_variance_filename = args.find(CHECK_MODEL_VARIANCE)->second.as<std::string>();
  m_segmentation_filename = args.find(CHECK_SEGMENTATION)->second.as<std::string>();
  m_partition_filename = args.find(CHECK_PARTITION)->second.as<std::string>();
  m_group_filename = args.find(CHECK_GROUPING)->second.as<std::string>();
  m_filtered_filename = args.find(CHECK_FILTERED)->second.as<std::string>();
  m_thresholded_filename = args.find(CHECK_THRESHOLDED)->second.as<std::string>();
  m_auto_aperture_filename = args.find(CHECK_AUTO_APERTURE)->second.as<std::string>();
  m_aperture_filename = args.find(CHECK_APERTURE)->second.as<std::string>();
  m_moffat_filename = args.find(CHECK_MOFFAT)->second.as<std::string>();
  m_psf_filename = args.find(CHECK_PSF)->second.as<std::string>();
}

} // SExtractor namespace


