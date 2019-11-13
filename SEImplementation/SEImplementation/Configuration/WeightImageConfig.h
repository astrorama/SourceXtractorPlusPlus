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
 * WeightImageConfig.h
 *
 *  Created on: Oct 7, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_WEIGHTIMAGECONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_WEIGHTIMAGECONFIG_H_

#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

class WeightImageConfig : public Euclid::Configuration::Configuration {

public:

  enum class WeightType {
    WEIGHT_TYPE_NONE,
    WEIGHT_TYPE_FROM_BACKGROUND,
    WEIGHT_TYPE_RMS,
    WEIGHT_TYPE_VARIANCE,
    WEIGHT_TYPE_WEIGHT,
  };

  virtual ~WeightImageConfig() = default;
  WeightImageConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  void initialize(const UserValues& args) override;

  std::shared_ptr<WeightImage> getWeightImage() const {
    return m_weight_image;
  }

  WeightType getWeightType() const {
    return m_weight_type;
  }

  bool isWeightAbsolute() const {
    return m_absolute_weight;
  }

  WeightImage::PixelType getWeightThreshold() const {
    return m_weight_threshold;
  }

  bool symmetryUsage() const {
    return m_symmetry_usage;
  }

  static std::shared_ptr<WeightImage> convertWeightMap(std::shared_ptr<WeightImage> weight_image, WeightType weight_type, WeightImage::PixelType scaling = 1);

private:

  std::shared_ptr<WeightImage> m_weight_image;
  WeightType m_weight_type;
  bool m_absolute_weight;
  WeightImage::PixelType m_weight_scaling;
  WeightImage::PixelType m_weight_threshold;
  bool m_symmetry_usage;
};


}


#endif /* _SEIMPLEMENTATION_CONFIGURATION_WEIGHTIMAGECONFIG_H_ */
