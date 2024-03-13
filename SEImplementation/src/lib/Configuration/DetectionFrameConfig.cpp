/** Copyright © 2019-2022 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include <iostream>

#include <boost/filesystem.hpp>

#include "Configuration/ConfigManager.h"

#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Image/ProcessedImage.h"

#include "SEImplementation/Background/BackgroundAnalyzerFactory.h"
#include "SEImplementation/Configuration/BackgroundConfig.h"
#include "SEImplementation/Configuration/DetectionImageConfig.h"
#include "SEImplementation/Configuration/WeightImageConfig.h"

#include "SEImplementation/CheckImages/CheckImages.h"

#include "SEImplementation/Configuration/DetectionFrameConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

DetectionFrameConfig::DetectionFrameConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<WeightImageConfig>();
  declareDependency<DetectionImageConfig>();
  declareDependency<BackgroundConfig>();
  declareDependency<BackgroundAnalyzerFactory>();
}

void DetectionFrameConfig::initialize(const UserValues& ) {

  for (size_t i=0; i<getDependency<DetectionImageConfig>().getExtensionsNb(); i++) {
    auto detection_image = getDependency<DetectionImageConfig>().getDetectionImage(i);
    auto detection_image_path = getDependency<DetectionImageConfig>().getDetectionImagePath();
    auto detection_image_coordinate_system = getDependency<DetectionImageConfig>().getCoordinateSystem(i);
    auto detection_image_gain = getDependency<DetectionImageConfig>().getGain(i);
    auto detection_image_saturation = getDependency<DetectionImageConfig>().getSaturation(i);
    auto interpolation_gap = getDependency<DetectionImageConfig>().getInterpolationGap(i);

    auto weight_image = getDependency<WeightImageConfig>().getWeightImage(i);
    auto weight_threshold = getDependency<WeightImageConfig>().getWeightThreshold(i);
    bool is_weight_absolute = getDependency<WeightImageConfig>().isWeightAbsolute();

    auto detection_frame = std::make_shared<DetectionImageFrame>(detection_image, weight_image,
        weight_threshold, detection_image_coordinate_system, detection_image_gain,
        detection_image_saturation, interpolation_gap);
    detection_frame->setLabel(boost::filesystem::basename(detection_image_path));

    auto background_analyzer = getDependency<BackgroundAnalyzerFactory>().createBackgroundAnalyzer();
    auto background_model = background_analyzer->analyzeBackground(detection_frame->getOriginalImage(), weight_image,
        ConstantImage<unsigned char>::create(detection_image->getWidth(), detection_image->getHeight(), false), detection_frame->getVarianceThreshold());

    detection_frame->setBackgroundLevel(background_model.getLevelMap(), background_model.getMedianRms());

    if (weight_image != nullptr) {
      if (is_weight_absolute) {
        detection_frame->setVarianceMap(weight_image);
      } else {
        // apply the rms scaling factor from the background
        auto bck_scaling_factor = background_model.getScalingFactor();
        auto scaled_image = MultiplyImage<SeFloat>::create(weight_image, bck_scaling_factor);
        detection_frame->setVarianceMap(scaled_image);
        detection_frame->setVarianceThreshold(detection_frame->getVarianceThreshold()*bck_scaling_factor);
      }
    } else {
      // re-set the variance check image to what's in the detection_frame()
      detection_frame->setVarianceMap(background_model.getVarianceMap());
    }

    const auto& background_config = getDependency<BackgroundConfig>();
    // Override background level and threshold if requested by the user
    if (background_config.isBackgroundLevelAbsolute()) {
      auto background = ConstantImage<DetectionImage::PixelType>::create(
          detection_image->getWidth(), detection_image->getHeight(), background_config.getBackgroundLevel());

      detection_frame->setBackgroundLevel(background, background_model.getMedianRms());

      CheckImages::getInstance().addBackgroundCheckImage(background);
    } else {
      CheckImages::getInstance().addBackgroundCheckImage(background_model.getLevelMap());
    }

    if (background_config.isDetectionThresholdAbsolute()) {
      detection_frame->setDetectionThreshold(background_config.getDetectionThreshold());
    }

    auto img_source = getDependency<DetectionImageConfig>().getImageSource(i);
    detection_frame->setMetadata(img_source->getMetadata());

    detection_frame->setHduIndex(i);

    CheckImages::getInstance().addVarianceCheckImage(detection_frame->getImage(FrameImageLayer::LayerVarianceMap));

    m_frames.emplace_back(detection_frame);
  }
}

}
