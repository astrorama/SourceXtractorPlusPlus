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
 * WeightImageConfig.cpp
 *
 *  Created on: Oct 7, 2016
 *      Author: mschefer
 */

#include <limits>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
using boost::regex;
using boost::regex_match;
using boost::smatch;

#include "Configuration/ConfigManager.h"

#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/Image/ProcessingImageSource.h"
#include "SEFramework/Image/ProcessedImage.h"
#include "SEFramework/FITS/FitsReader.h"
#include "SEFramework/FITS/FitsImageSource.h"

#include "SEImplementation/Configuration/DetectionImageConfig.h"

#include "SEImplementation/Configuration/WeightImageConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

static const std::string WEIGHT_IMAGE {"weight-image" };
static const std::string WEIGHT_TYPE {"weight-type" };
static const std::string WEIGHT_ABSOLUTE {"weight-absolute" };
static const std::string WEIGHT_SCALING {"weight-scaling" };
static const std::string WEIGHT_THRESHOLD {"weight-threshold" };
static const std::string WEIGHT_SYMMETRYUSAGE {"weight-use-symmetry" };

WeightImageConfig::WeightImageConfig(long manager_id) :
    Configuration(manager_id),
    m_weight_type(WeightType::WEIGHT_TYPE_FROM_BACKGROUND),
    m_absolute_weight(false),
    m_weight_scaling(1),
    m_weight_threshold(0),
    m_symmetry_usage(true) {

  declareDependency<DetectionImageConfig>();
}

std::map<std::string, Configuration::OptionDescriptionList> WeightImageConfig::getProgramOptions() {
  return { {"Weight image", {
      {WEIGHT_IMAGE.c_str(), po::value<std::string>()->default_value(""),
          "Path to a fits format image to be used as weight image."},
      {WEIGHT_ABSOLUTE.c_str(), po::value<bool>()->default_value(false),
          "Is the weight map provided as absolute values or relative to background."},
      {WEIGHT_TYPE.c_str(), po::value<std::string>()->default_value("none"),
          "Weight image type [none|background|rms|variance|weight]."},
      {WEIGHT_SCALING.c_str(), po::value<double>()->default_value(1.0),
          "Weight map scaling factor."},
      {WEIGHT_THRESHOLD.c_str(), po::value<double>(),
          "Threshold for pixels to be considered bad pixels. In same units as weight map."},
      {WEIGHT_SYMMETRYUSAGE.c_str(), po::value<bool>()->default_value(true),
          "Use object symmetry to replace pixels above the weight threshold for photometry."},
  }}};
}

static WeightImage::PixelType computeWeightThreshold(WeightImageConfig::WeightType weight_type, double threshold) {
  using WeightType = WeightImageConfig::WeightType;

  switch (weight_type) {
    default:
    case WeightType::WEIGHT_TYPE_FROM_BACKGROUND:
    case WeightType::WEIGHT_TYPE_RMS:
      return threshold * threshold;
    case WeightType::WEIGHT_TYPE_VARIANCE:
      return threshold;
    case WeightType::WEIGHT_TYPE_WEIGHT:
      if (threshold > 0) {
        return 1.0 / threshold;
      } else {
        return std::numeric_limits<WeightImage::PixelType>::max();
      }
  }
}

void WeightImageConfig::initialize(const UserValues& args) {
  static const std::map<std::string, WeightType> WEIGHT_MAP{
    {"NONE",       WeightType::WEIGHT_TYPE_NONE},
    {"BACKGROUND", WeightType::WEIGHT_TYPE_FROM_BACKGROUND},
    {"RMS",        WeightType::WEIGHT_TYPE_RMS},
    {"VARIANCE",   WeightType::WEIGHT_TYPE_VARIANCE},
    {"WEIGHT",     WeightType::WEIGHT_TYPE_WEIGHT}
  };

  m_absolute_weight = args.find(WEIGHT_ABSOLUTE)->second.as<bool>();
  m_symmetry_usage = args.find(WEIGHT_SYMMETRYUSAGE)->second.as<bool>();

  auto weight_image_filename = args.find(WEIGHT_IMAGE)->second.as<std::string>();

  auto weight_type_name = boost::to_upper_copy(args.at(WEIGHT_TYPE).as<std::string>());
  auto weight_iter = WEIGHT_MAP.find(weight_type_name);
  if (weight_iter == WEIGHT_MAP.end()) {
    throw Elements::Exception() << "Unknown weight map type : " << weight_type_name;
  }
  m_weight_type = weight_iter->second;
  m_weight_scaling = args.find(WEIGHT_SCALING)->second.as<double>();

  if (args.count(WEIGHT_THRESHOLD) != 0) {
    auto threshold = args.find(WEIGHT_THRESHOLD)->second.as<double>();
    m_weight_threshold = computeWeightThreshold(m_weight_type, threshold);
  } else {
    m_weight_threshold = std::numeric_limits<WeightImage::PixelType>::max();
  }

  // some safeguards that the user provides reasonable input and gets defined results
  if (weight_image_filename != "" && m_weight_type == WeightType::WEIGHT_TYPE_FROM_BACKGROUND)
    throw Elements::Exception() << "Please give an appropriate weight type for image: " << weight_image_filename;
  if (weight_image_filename != "" && m_weight_type == WeightType::WEIGHT_TYPE_NONE)
    throw Elements::Exception() << "Please give an appropriate weight type for image: " << weight_image_filename;
  if (m_absolute_weight && weight_image_filename == "")
    throw Elements::Exception() << "Setting absolute weight but providing *no* weight image does not make sense.";

  if (weight_image_filename != "") {
    boost::regex hdu_regex(".*\\[[0-9]*\\]$");

    for (int i=0;; i++) {
      std::shared_ptr<FitsImageSource> fits_image_source;
      if (boost::regex_match(weight_image_filename, hdu_regex)) {
        if (i==0) {
          fits_image_source = std::make_shared<FitsImageSource>(weight_image_filename, 0, ImageTile::FloatImage);
        } else {
          break;
        }
      } else {
        try {
          fits_image_source = std::make_shared<FitsImageSource>(weight_image_filename, i+1, ImageTile::FloatImage);
        } catch (...) {
          if (i==0) {
            // Skip past primary HDU if it doesn't have an image
            continue;
          } else {
            if (m_weight_images.size() == 0) {
              throw Elements::Exception() << "Can't find 2D image in FITS file: " << weight_image_filename;
            }
            break;
          }
        }
      }

      std::shared_ptr<WeightImage> weight_image = BufferedImage<DetectionImage::PixelType>::create(fits_image_source);
      weight_image = convertWeightMap(weight_image, m_weight_type, m_weight_scaling);

      // we should have a corresponding detection image
      auto flux_scale = getDependency<DetectionImageConfig>().getOriginalFluxScale(m_weight_images.size());

      WeightImage::PixelType scaled_weight_threshold = m_weight_threshold;
      if (flux_scale != 1. && m_absolute_weight) {
        weight_image = MultiplyImage<WeightImage::PixelType>::create(weight_image, flux_scale * flux_scale);
        if (scaled_weight_threshold < std::numeric_limits<WeightImage::PixelType>::max()){
          // adjust the weight threshold
          scaled_weight_threshold *= flux_scale;
        }
      }

      m_weight_images.emplace_back(weight_image);
      m_scaled_weight_thresholds.emplace_back(scaled_weight_threshold);
    }
  }
}

class WeightMapImageSource : public ProcessingImageSource<WeightImage::PixelType> {
public:
  WeightMapImageSource(std::shared_ptr<Image<WeightImage::PixelType>> image, WeightImageConfig::WeightType weight_type, WeightImage::PixelType scaling)
    : ProcessingImageSource<DetectionImage::PixelType>(image), m_weight_type(weight_type), m_scaling(scaling)
       {}

protected:

  std::string getRepr() const override {
    return "WeightMapImageSource(" + getImageRepr() + ")";
  }

  void generateTile(const std::shared_ptr<Image<WeightImage::PixelType>>& image,
                    ImageTileWithType<WeightImage::PixelType>& tile, int x, int y, int width, int height) const final {
    auto image_chunk = image->getChunk(x, y, width, height);
    switch (m_weight_type) {
      case WeightImageConfig::WeightType::WEIGHT_TYPE_RMS:
        generateFromRms(tile, width, height, *image_chunk);
        break;
      case WeightImageConfig::WeightType::WEIGHT_TYPE_VARIANCE:
        generateFromVariance(tile, width, height, *image_chunk);
        break;
      case WeightImageConfig::WeightType::WEIGHT_TYPE_WEIGHT:
        generateFromWeight(tile, width, height, *image_chunk);
        break;
      default:
      case WeightImageConfig::WeightType::WEIGHT_TYPE_FROM_BACKGROUND:
        assert(false);
        break;
    }
  }

  void generateFromWeight(ImageTileWithType<WeightImage::PixelType>& tile, int width, int height,
                          const ImageChunk<WeightImage::PixelType>& image_chunk) const {
    auto& tile_image = *tile.getImage();
   for (int iy = 0; iy < height; iy++) {
      for (int ix = 0; ix < width; ix++) {
        auto value = image_chunk.getValue(ix, iy) * m_scaling;
        if (value > 0) {
          tile_image.setValue(ix, iy, 1.0 / value);
        }
        else {
          tile_image.setValue(ix, iy, std::numeric_limits<WeightImage::PixelType>::infinity());
        }
      }
    }
  }

  void generateFromVariance(ImageTileWithType<WeightImage::PixelType>& tile, int width, int height,
                            const ImageChunk<WeightImage::PixelType>& image_chunk) const {
    auto& tile_image = *tile.getImage();
    for (int iy = 0; iy < height; iy++) {
      for (int ix = 0; ix < width; ix++) {
        auto value = image_chunk.getValue(ix, iy) * m_scaling;
        tile_image.setValue(ix, iy, value);
      }
    }
  }

  void generateFromRms(ImageTileWithType<WeightImage::PixelType>& tile, int width, int height,
                       const ImageChunk<WeightImage::PixelType>& image_chunk) const {
    auto& tile_image = *tile.getImage();
    for (int iy = 0; iy < height; iy++) {
      for (int ix = 0; ix < width; ix++) {
        auto value = image_chunk.getValue(ix, iy) * m_scaling;
        tile_image.setValue(ix, iy, value * value);
      }
    }
  }

private:
  WeightImageConfig::WeightType m_weight_type;
  WeightImage::PixelType m_scaling;
};


std::shared_ptr<WeightImage>
WeightImageConfig::convertWeightMap(std::shared_ptr<WeightImage> weight_image, WeightType weight_type,
                                    WeightImage::PixelType scaling) {

  if (weight_type == WeightType::WEIGHT_TYPE_FROM_BACKGROUND) {
    return nullptr;
  }
  else if (weight_type == WeightType::WEIGHT_TYPE_NONE) {
    return nullptr;
  }
  else {
    auto result_image = BufferedImage<WeightImage::PixelType>::create(
      std::make_shared<WeightMapImageSource>(weight_image, weight_type, scaling));
    return result_image;
  }
}

}

