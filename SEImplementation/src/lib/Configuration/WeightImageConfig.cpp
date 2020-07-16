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

#include "Configuration/ConfigManager.h"

#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/Image/ProcessingImageSource.h"
#include "SEFramework/Image/ProcessedImage.h"
#include "SEFramework/FITS/FitsReader.h"

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

void WeightImageConfig::initialize(const UserValues& args) {
  m_absolute_weight = args.find(WEIGHT_ABSOLUTE)->second.as<bool>();
  m_symmetry_usage = args.find(WEIGHT_SYMMETRYUSAGE)->second.as<bool>();
  auto weight_image_filename = args.find(WEIGHT_IMAGE)->second.as<std::string>();
  if (weight_image_filename != "") {
    m_weight_image = FitsReader<WeightImage::PixelType>::readFile(weight_image_filename);
  }

  auto weight_type_name = boost::to_upper_copy(args.at(WEIGHT_TYPE).as<std::string>());
  if (weight_type_name == "NONE") {
    m_weight_type = WeightType::WEIGHT_TYPE_NONE;
  } else if (weight_type_name == "BACKGROUND") {
    m_weight_type = WeightType::WEIGHT_TYPE_FROM_BACKGROUND;
  } else if (weight_type_name == "RMS") {
    m_weight_type = WeightType::WEIGHT_TYPE_RMS;
  } else if (weight_type_name == "VARIANCE") {
    m_weight_type = WeightType::WEIGHT_TYPE_VARIANCE;
  } else if (weight_type_name == "WEIGHT") {
    m_weight_type = WeightType::WEIGHT_TYPE_WEIGHT;
  } else {
    throw Elements::Exception() << "Unknown weight map type : " << weight_type_name;
  }

  m_weight_scaling = args.find(WEIGHT_SCALING)->second.as<double>();

  if (m_weight_image != nullptr) {
    m_weight_image = convertWeightMap(m_weight_image, m_weight_type, m_weight_scaling);

    auto flux_scale = getDependency<DetectionImageConfig>().getOriginalFluxScale();
    if (flux_scale != 1. && m_absolute_weight) {
      m_weight_image = MultiplyImage<WeightImage::PixelType>::create(m_weight_image, flux_scale * flux_scale);
    }
  }

  if (args.count(WEIGHT_THRESHOLD) != 0) {
    auto threshold = args.find(WEIGHT_THRESHOLD)->second.as<double>();
    switch (m_weight_type) {
      default:
      case WeightType::WEIGHT_TYPE_FROM_BACKGROUND:
      case WeightType::WEIGHT_TYPE_RMS:
        m_weight_threshold = threshold * threshold;
        break;
      case WeightType::WEIGHT_TYPE_VARIANCE:
        m_weight_threshold = threshold;
        break;
      case WeightType::WEIGHT_TYPE_WEIGHT:
        if (threshold>0)
          m_weight_threshold = 1.0 / threshold;
        else
          m_weight_threshold = std::numeric_limits<WeightImage::PixelType>::max();
        break;
    }
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
                    ImageTile<DetectionImage::PixelType>& tile, int x, int y, int width, int height) const final {
    auto image_chunk = image->getChunk(x, y, width, height);
    switch (m_weight_type) {
      case WeightImageConfig::WeightType::WEIGHT_TYPE_RMS:
        for (int iy = 0; iy < height; iy++) {
          for (int ix = 0; ix < width; ix++) {
            auto value = image_chunk->getValue(ix, iy) * m_scaling;
            tile.getImage()->setValue(ix, iy, value * value);
          }
        }
        break;
      case WeightImageConfig::WeightType::WEIGHT_TYPE_VARIANCE:
        for (int iy = 0; iy < height; iy++) {
          for (int ix = 0; ix < width; ix++) {
            auto value = image_chunk->getValue(ix, iy) * m_scaling;
            tile.getImage()->setValue(ix, iy, value);
          }
        }
        break;
      case WeightImageConfig::WeightType::WEIGHT_TYPE_WEIGHT:
        for (int iy = 0; iy < height; iy++) {
          for (int ix = 0; ix < width; ix++) {
            auto value = image_chunk->getValue(ix, iy) * m_scaling;
            if (value > 0) {
              tile.getImage()->setValue(ix, iy, 1.0 / value);
            } else {
              tile.getImage()->setValue(ix, iy, std::numeric_limits<WeightImage::PixelType>::max());
            }
          }
        }
        break;
      default:
      case WeightImageConfig::WeightType::WEIGHT_TYPE_FROM_BACKGROUND:
        assert(false);
        break;
    }
  }

private:
  WeightImageConfig::WeightType m_weight_type;
  WeightImage::PixelType m_scaling;
};


std::shared_ptr<WeightImage> WeightImageConfig::convertWeightMap(std::shared_ptr<WeightImage> weight_image, WeightType weight_type, WeightImage::PixelType scaling) {

  if (weight_type == WeightType::WEIGHT_TYPE_FROM_BACKGROUND) {
    return nullptr;
  } else if (weight_type == WeightType::WEIGHT_TYPE_NONE) {
    return nullptr;
  } else {
    auto result_image = BufferedImage<WeightImage::PixelType>::create(std::make_shared<WeightMapImageSource>(weight_image, weight_type, scaling));
    return result_image;
  }
}

}

