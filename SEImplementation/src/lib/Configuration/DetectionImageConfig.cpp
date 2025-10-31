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
#include <memory>
using boost::regex;
using boost::regex_match;
using boost::smatch;

#include "SEFramework/CoordinateSystem/WCS.h"
#include "SEFramework/Image/BufferedImage.h"
#include "SEFramework/Image/ImageFileReader.h"
#include "SEFramework/Image/ProcessedImage.h"
#include "SEFramework/FITS/FitsImageSource.h"

#ifdef WITH_ASDF
#include "SEFramework/ASDF/AsdfImageSource.h"
#endif

#include "SEImplementation/Configuration/DetectionImageConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

static const std::string DETECTION_IMAGE { "detection-image" };
static const std::string REFERENCE_IMAGE { "reference-image" };
static const std::string DETECTION_IMAGE_GAIN { "detection-image-gain" };
static const std::string DETECTION_IMAGE_FLUX_SCALE {"detection-image-flux-scale"};
static const std::string DETECTION_IMAGE_SATURATION { "detection-image-saturation" };
static const std::string DETECTION_IMAGE_INTERPOLATION { "detection-image-interpolation" };
static const std::string DETECTION_IMAGE_INTERPOLATION_GAP { "detection-image-interpolation-gap" };

#ifdef WITH_ASDF
static const std::string DETECTION_IMAGE_ASDF_WCS_PATH { "detection-image-asdf-wcs-path" };
static const std::string REFERENCE_IMAGE_ASDF_WCS_PATH { "reference-image-asdf-wcs-path" };
#endif

DetectionImageConfig::DetectionImageConfig(long manager_id) : Configuration(manager_id)
{}

std::map<std::string, Configuration::OptionDescriptionList> DetectionImageConfig::getProgramOptions() {
  return { {"Detection image", {
      {DETECTION_IMAGE.c_str(), po::value<std::string>(),
      // NOTE: In principle ImageFileReader can be used to programmatically
      // build a string listing the supported file types, but as currently
      // there are only two and one of them only with optional support we
      // can use an ifdef for now.
#ifdef WITH_ASDF
          "Path to a FITS or ASDF format image to be used as detection image."},
#else
          "Path to a FITS format image to be used as detection image."},
#endif
      {REFERENCE_IMAGE.c_str(), po::value<std::string>(),
#ifdef WITH_ASDF
          "Path to a FITS or ASDF format image to be used as coordinates reference only."},
#else
          "Path to a FITS or ASDF format image to be used as coordinates reference only."},
#endif
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
#ifdef WITH_ASDF
      ,
      {DETECTION_IMAGE_ASDF_WCS_PATH.c_str(), po::value<std::string>(),
          "When reading from an ASDF file, the JSON path to the WCS to use for the detection image"},
      {REFERENCE_IMAGE_ASDF_WCS_PATH.c_str(), po::value<std::string>(),
          "When reading from an ASDF file, the JSON path to the WCS to use for the reference image"}
#endif
  }}};
}

void DetectionImageConfig::initialize(const UserValues& args) {

  if (args.find(DETECTION_IMAGE) == args.end()) {
    // Running without a detection image

    // Check if a reference image is provided
    if (args.find(REFERENCE_IMAGE) != args.end()) {
      DetectionImageExtension extension;

      auto image_reader = ImageFileReader::create(
        args.find(REFERENCE_IMAGE)->second.as<std::string>());
      auto reference_image_source = image_reader->get(0);
#ifndef WITH_ASDF
      extension.m_coordinate_system = std::make_shared<WCS>(*reference_image_source);
#else
      if (auto asdf_src = std::dynamic_pointer_cast<AsdfImageSource>(reference_image_source)) {
        std::optional<std::string> wcs_path;
        if (auto it = args.find(REFERENCE_IMAGE_ASDF_WCS_PATH); it != args.end())
            wcs_path = it->second.as<std::string>();
        extension.m_coordinate_system = std::make_shared<WCS>(*asdf_src, wcs_path);
      } else {
        extension.m_coordinate_system = std::make_shared<WCS>(*reference_image_source);
      }
#endif
      m_extensions.emplace_back(std::move(extension));

      m_is_reference_image = true;
    }

    return;
  }

  if (args.find(REFERENCE_IMAGE) != args.end()) {
    throw Elements::Exception() << "Either detection or reference image can be provided, not both";
  }

  m_detection_image_path = args.find(DETECTION_IMAGE)->second.as<std::string>();

  auto image_reader = ImageFileReader::create(m_detection_image_path);

  for (const auto& img_source: image_reader->iter(ImageTile::FloatImage)) {
    DetectionImageExtension extension = DetectionImageExtension::create(img_source, args);
    m_extensions.emplace_back(std::move(extension));
  }
}


DetectionImageConfig::DetectionImageExtension::DetectionImageExtension(
    std::shared_ptr<ImageSource> image_source, double gain, double saturation,
    double flux_scale, int interpolation_gap) {
  init(image_source, gain, saturation, flux_scale, interpolation_gap);
  m_coordinate_system = std::make_shared<WCS>(*image_source);
  rescale();
}


DetectionImageConfig::DetectionImageExtension::DetectionImageExtension(
    std::shared_ptr<FitsImageSource> fits_image_source, double gain, double saturation,
    double flux_scale, int interpolation_gap) {
  init(fits_image_source, gain, saturation, flux_scale, interpolation_gap);
  m_coordinate_system = std::make_shared<WCS>(*fits_image_source);
  auto img_metadata = fits_image_source->getMetadata();

  if (img_metadata.count("GAIN")){
      // read the keyword GAIN from the metadata
      if (double* double_gain = boost::get<double>(&img_metadata.at("GAIN").m_value)){
        m_gain = *double_gain;
      } else if (int64_t *int64_gain = boost::get<int64_t>(&img_metadata.at("GAIN").m_value)){
        m_gain = (double) *int64_gain;
      }
      else {
        throw Elements::Exception() << "Keyword GAIN must be either float or int!";
      }
  }

  if (img_metadata.count("FLXSCALE")) {
    // read the keyword FLXSCALE from the metadata
    if (double* f_scale = boost::get<double>(&img_metadata.at("FLXSCALE").m_value)){
      m_flux_scale = *f_scale;
    } else if (int64_t *int64_f_scale = boost::get<int64_t>(&img_metadata.at("FLXSCALE").m_value)){
      m_flux_scale = (double) *int64_f_scale;
    }
    else {
      throw Elements::Exception() << "Keyword FLXSCALE must be either float or int!";
    }
  }

  if (img_metadata.count("SATURATE")){
    // read the keyword SATURATE from the metadata
    if (double* double_saturate = boost::get<double>(&img_metadata.at("SATURATE").m_value)){
      m_saturation = *double_saturate;
    } else if (int64_t *int64_saturate = boost::get<int64_t>(&img_metadata.at("SATURATE").m_value)){
      m_saturation = (double) *int64_saturate;
    }
    else {
      throw Elements::Exception() << "Keyword SATURATE must be either float or int!";
    }
  }

  rescale();
}


#ifdef WITH_ASDF
/**
 * Special case for loading from ASDF when enabled, to use the detection-image-asdf-wcs-path
 * option if given
 */
DetectionImageConfig::DetectionImageExtension::DetectionImageExtension(
    std::shared_ptr<AsdfImageSource> asdf_image_source, double gain, double saturation,
    double flux_scale, int interpolation_gap, std::optional<std::string> wcs_path) {
  init(asdf_image_source, gain, saturation, flux_scale, interpolation_gap);
  m_coordinate_system = std::make_shared<WCS>(*asdf_image_source, wcs_path);
  rescale();
}


#endif /* WITH_ASDF */


void DetectionImageConfig::DetectionImageExtension::init(
    std::shared_ptr<ImageSource> image_source, double gain, double saturation,
    double flux_scale, int interpolation_gap) {
  m_image_source = image_source;
  m_detection_image = BufferedImage<DetectionImage::PixelType>::create(image_source);
  m_gain = gain;
  m_saturation = saturation;
  m_flux_scale = flux_scale;
  m_interpolation_gap = interpolation_gap;
}


DetectionImageConfig::DetectionImageExtension DetectionImageConfig::DetectionImageExtension::create(
    std::shared_ptr<ImageSource> image_source, const UserValues& args) {
  double gain = (args.find(DETECTION_IMAGE_GAIN) != args.end())
    ? args.find(DETECTION_IMAGE_GAIN)->second.as<double>()
    : 0.0;

  double saturation = (args.find(DETECTION_IMAGE_SATURATION) != args.end())
    ? args.find(DETECTION_IMAGE_SATURATION)->second.as<double>()
    : 0.0;

  double flux_scale = (args.find(DETECTION_IMAGE_FLUX_SCALE) != args.end())
    ? args.find(DETECTION_IMAGE_FLUX_SCALE)->second.as<double>()
    : 1.0;

  int interpolation_gap = args.find(DETECTION_IMAGE_INTERPOLATION)->second.as<bool>()
    ? std::max(0, args.find(DETECTION_IMAGE_INTERPOLATION_GAP)->second.as<int>())
    : 0;

  if (auto fits_src = std::dynamic_pointer_cast<FitsImageSource>(image_source)) {
    return DetectionImageExtension(fits_src, gain, saturation, flux_scale, interpolation_gap);
  }
#ifdef WITH_ASDF
  else if (auto asdf_src = std::dynamic_pointer_cast<AsdfImageSource>(image_source)) {
    std::optional<std::string> wcs_path;
    if (auto it = args.find(DETECTION_IMAGE_ASDF_WCS_PATH); it != args.end())
        wcs_path = it->second.as<std::string>();
    return DetectionImageExtension(asdf_src, gain, saturation, flux_scale, interpolation_gap,
                                   wcs_path);
  }
#endif

  return DetectionImageExtension(image_source, gain, saturation, flux_scale, interpolation_gap);
}


void DetectionImageConfig::DetectionImageExtension::rescale() {
  // Adapt image and parameters to take flux_scale into consideration
  if (m_flux_scale != 1.0) {
    m_detection_image =
        MultiplyImage<DetectionImage::PixelType>::create(m_detection_image, m_flux_scale);
    m_gain /= m_flux_scale;
    m_saturation *= m_flux_scale;
  }
}


std::string DetectionImageConfig::getDetectionImagePath() const {
  return m_detection_image_path;
}

std::shared_ptr<DetectionImage> DetectionImageConfig::getDetectionImage(size_t index) const {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getDetectionImage() call on not initialized DetectionImageConfig";
  }
  if (m_is_reference_image) {
    throw Elements::Exception() << "Trying to access detection image but only a reference image was provided";
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



