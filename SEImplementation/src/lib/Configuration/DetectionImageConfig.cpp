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
/**
 * @file src/lib/DetectionImageConfig.cpp
 * @date 06/06/16
 * @author mschefer
 */
#include "Configuration/ConfigManager.h"

#include <boost/regex.hpp>
using boost::regex;
using boost::regex_match;
using boost::smatch;

#include <SEFramework/Image/ProcessedImage.h>
#include <SEFramework/Image/BufferedImage.h>
#include "SEFramework/FITS/FitsImageSource.h"

#include "SEFramework/CoordinateSystem/WCS.h"

#include "SEImplementation/Configuration/DetectionImageConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

static const std::string DETECTION_IMAGE { "detection-image" };
static const std::string DETECTION_IMAGE_GAIN { "detection-image-gain" };
static const std::string DETECTION_IMAGE_FLUX_SCALE {"detection-image-flux-scale"};
static const std::string DETECTION_IMAGE_SATURATION { "detection-image-saturation" };
static const std::string DETECTION_IMAGE_INTERPOLATION { "detection-image-interpolation" };
static const std::string DETECTION_IMAGE_INTERPOLATION_GAP { "detection-image-interpolation-gap" };

DetectionImageConfig::DetectionImageConfig(long manager_id) : Configuration(manager_id)
{}

std::map<std::string, Configuration::OptionDescriptionList> DetectionImageConfig::getProgramOptions() {
  return { {"Detection image", {
      {DETECTION_IMAGE.c_str(), po::value<std::string>(),
          "Path to a fits format image to be used as detection image."},
      {DETECTION_IMAGE_GAIN.c_str(), po::value<double>(),
          "Detection image gain in e-/ADU (0 = infinite gain)"},
      {DETECTION_IMAGE_FLUX_SCALE.c_str(), po::value<double>(),
          "Detection image flux scale"},
      {DETECTION_IMAGE_SATURATION.c_str(), po::value<double>(),
          "Detection image saturation level (0 = no saturation)"},
      {DETECTION_IMAGE_INTERPOLATION.c_str(), po::value<bool>()->default_value(true),
          "Interpolate bad pixels in detection image"},
      {DETECTION_IMAGE_INTERPOLATION_GAP.c_str(), po::value<int>()->default_value(5),
          "Maximum number if pixels to interpolate over"}
  }}};
}

void DetectionImageConfig::initialize(const UserValues& args) {
  // Normally we would define this one as required, but then --list-output-properties would be
  // unusable unless we also specify --detection-image, which is not very intuitive.
  // For this reason, we check for its existence here
  if (args.find(DETECTION_IMAGE) == args.end()) {
    throw Elements::Exception() << "'--" << DETECTION_IMAGE << "' is required but missing";
  }

  m_detection_image_path = args.find(DETECTION_IMAGE)->second.as<std::string>();

  boost::regex hdu_regex(".*\\[[0-9]*\\]$");

  for (int i=0;; i++) {
    DetectionImageExtension extension;

    std::shared_ptr<FitsImageSource> fits_image_source;
    if (boost::regex_match(m_detection_image_path, hdu_regex)) {
      if (i==0) {
        fits_image_source = std::make_shared<FitsImageSource>(m_detection_image_path, 0, ImageTile::FloatImage);
      } else {
        break;
      }
    } else {
      try {
        fits_image_source = std::make_shared<FitsImageSource>(m_detection_image_path, i+1, ImageTile::FloatImage);
      } catch (...) {
        if (i==0) {
          // Skip past primary HDU if it doesn't have an image
          continue;
        } else {
          if (m_extensions.size() == 0) {
            throw Elements::Exception() << "Can't find 2D image in FITS file: " << m_detection_image_path;
          }
          break;
        }
      }
    }

    extension.m_image_source = fits_image_source;
    extension.m_detection_image = BufferedImage<DetectionImage::PixelType>::create(fits_image_source);
    extension.m_coordinate_system = std::make_shared<WCS>(*fits_image_source);

    double detection_image_gain = 0, detection_image_saturate = 0;
    auto img_metadata = fits_image_source->getMetadata();

    if (img_metadata.count("GAIN")){
        // read the keyword GAIN from the metadata
        if (double* double_gain = boost::get<double>(&img_metadata.at("GAIN").m_value)){
          detection_image_gain = *double_gain;
        } else if (int64_t *int64_gain = boost::get<int64_t>(&img_metadata.at("GAIN").m_value)){
          detection_image_gain = (double) *int64_gain;
        }
        else {
          throw Elements::Exception() << "Keyword GAIN must be either float or int!";
        }
    }

    if (img_metadata.count("SATURATE")){
      // read the keyword SATURATE from the metadata
      if (double* double_saturate = boost::get<double>(&img_metadata.at("SATURATE").m_value)){
        detection_image_saturate = *double_saturate;
      } else if (int64_t *int64_saturate = boost::get<int64_t>(&img_metadata.at("SATURATE").m_value)){
        detection_image_saturate = (double) *int64_saturate;
      }
      else {
        throw Elements::Exception() << "Keyword SATURATE must be either float or int!";
      }
    }

    if (args.find(DETECTION_IMAGE_FLUX_SCALE) != args.end()) {
      extension.m_flux_scale = args.find(DETECTION_IMAGE_FLUX_SCALE)->second.as<double>();
    }
    else if (img_metadata.count("FLXSCALE")) {
      // read the keyword FLXSCALE from the metadata
      if (double* f_scale = boost::get<double>(&img_metadata.at("FLXSCALE").m_value)){
        extension.m_flux_scale = *f_scale;
      } else if (int64_t *int64_f_scale = boost::get<int64_t>(&img_metadata.at("FLXSCALE").m_value)){
        extension.m_flux_scale = (double) *int64_f_scale;
      }
      else {
        throw Elements::Exception() << "Keyword FLXSCALE must be either float or int!";
      }
    }

    if (args.find(DETECTION_IMAGE_GAIN) != args.end()) {
      extension.m_gain = args.find(DETECTION_IMAGE_GAIN)->second.as<double>();
    }
    else {
      extension.m_gain = detection_image_gain;
    }

    if (args.find(DETECTION_IMAGE_SATURATION) != args.end()) {
      extension.m_saturation = args.find(DETECTION_IMAGE_SATURATION)->second.as<double>();
    }
    else {
      extension.m_saturation = detection_image_saturate;
    }

    extension.m_interpolation_gap = args.find(DETECTION_IMAGE_INTERPOLATION)->second.as<bool>() ?
        std::max(0, args.find(DETECTION_IMAGE_INTERPOLATION_GAP)->second.as<int>()) : 0;

    // Adapt image and parameters to take flux_scale into consideration
    if (extension.m_flux_scale != 1.0) {
      extension.m_detection_image =
          MultiplyImage<DetectionImage::PixelType>::create(extension.m_detection_image, extension.m_flux_scale);
      extension.m_gain /= extension.m_flux_scale;
      extension.m_saturation *= extension.m_flux_scale;
    }

    m_extensions.emplace_back(std::move(extension));
  }
}

std::string DetectionImageConfig::getDetectionImagePath() const {
  return m_detection_image_path;
}

std::shared_ptr<DetectionImage> DetectionImageConfig::getDetectionImage(size_t index) const {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getDetectionImage() call on not initialized DetectionImageConfig";
  }
  return m_extensions.at(index).m_detection_image;
}

std::shared_ptr<CoordinateSystem> DetectionImageConfig::getCoordinateSystem(size_t index) const {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getCoordinateSystem() call on not initialized DetectionImageConfig";
  }
  return m_extensions.at(index).m_coordinate_system;
}

} // SourceXtractor namespace



