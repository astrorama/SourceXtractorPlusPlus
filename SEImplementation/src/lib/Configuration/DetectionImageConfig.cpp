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

DetectionImageConfig::DetectionImageConfig(long manager_id) : Configuration(manager_id),
    m_gain(0), m_saturation(0), m_flux_scale(1.0), m_interpolation_gap(0) {
}

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
  auto fits_image_source = std::make_shared<FitsImageSource<DetectionImage::PixelType>>(m_detection_image_path);
  m_image_source = fits_image_source;
  m_detection_image = BufferedImage<DetectionImage::PixelType>::create(m_image_source);
  m_coordinate_system = std::make_shared<WCS>(*fits_image_source);

  double detection_image_gain = 0, detection_image_saturate = 0;
  auto img_metadata = m_image_source->getMetadata();

  if (img_metadata.count("GAIN"))
    detection_image_gain = boost::get<double>(img_metadata.at("GAIN").m_value);
  if (img_metadata.count("SATURATE"))
    detection_image_saturate = boost::get<double>(img_metadata.at("SATURATE").m_value);

  if (args.find(DETECTION_IMAGE_FLUX_SCALE) != args.end()) {
    m_flux_scale = args.find(DETECTION_IMAGE_FLUX_SCALE)->second.as<double>();
  }
  else if (img_metadata.count("FLXSCALE")) {
    m_flux_scale = boost::get<double>(img_metadata.at("FLXSCALE").m_value);
  }

  if (args.find(DETECTION_IMAGE_GAIN) != args.end()) {
    m_gain = args.find(DETECTION_IMAGE_GAIN)->second.as<double>();
  }
  else {
    m_gain = detection_image_gain;
  }

  if (args.find(DETECTION_IMAGE_SATURATION) != args.end()) {
    m_saturation = args.find(DETECTION_IMAGE_SATURATION)->second.as<double>();
  }
  else {
    m_saturation = detection_image_saturate;
  }

  m_interpolation_gap = args.find(DETECTION_IMAGE_INTERPOLATION)->second.as<bool>() ?
      std::max(0, args.find(DETECTION_IMAGE_INTERPOLATION_GAP)->second.as<int>()) : 0;

  // Adapt image and parameters to take flux_scale into consideration
  if (m_flux_scale != 1.0) {
    m_detection_image = MultiplyImage<DetectionImage::PixelType>::create(m_detection_image, m_flux_scale);
    m_gain /= m_flux_scale;
    m_saturation *= m_flux_scale;
  }
}

std::string DetectionImageConfig::getDetectionImagePath() const {
  return m_detection_image_path;
}

std::shared_ptr<DetectionImage> DetectionImageConfig::getDetectionImage() const {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getDetectionImage() call on not initialized DetectionImageConfig";
  }
  return m_detection_image;
}

std::shared_ptr<CoordinateSystem> DetectionImageConfig::getCoordinateSystem() const {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getCoordinateSystem() call on not initialized DetectionImageConfig";
  }
  return m_coordinate_system;
}

} // SourceXtractor namespace



