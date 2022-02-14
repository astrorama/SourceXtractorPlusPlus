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

#ifndef _SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEINFO_DETECTIONFRAMEINFO_H_
#define _SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEINFO_DETECTIONFRAMEINFO_H_

#include "SEUtils/Types.h"

#include "SEFramework/Property/Property.h"
#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SourceXtractor {

class DetectionFrameInfo : public Property {

public:
  virtual ~DetectionFrameInfo() = default;

  DetectionFrameInfo(int width, int height, double gain, double saturation,
      double variance_threshold, double background_median_rms, const std::map<std::string,
      MetadataEntry>& metadata = std::map<std::string, MetadataEntry>())
      : m_width(width), m_height(height),
        m_gain(gain), m_saturation(saturation),
        m_variance_threshold(variance_threshold), m_background_median_rms(background_median_rms),
        m_metadata(metadata) {}

  double getGain() const {
    return m_gain;
  }

  double getSaturation() const {
    return m_saturation;
  }

  int getWidth() const {
    return m_width;
  }

  int getHeight() const {
    return m_height;
  }

  SeFloat getVarianceThreshold() const {
    return m_variance_threshold;
  }

  SeFloat getBackgroundMedianRms() const {
    return m_background_median_rms;
  }

  // Metadata of the original image
  const std::map<std::string, MetadataEntry>& getMetadata() const {
    return m_metadata;
  };

private:
  int m_width;
  int m_height;

  double m_gain;
  double m_saturation;

  double m_variance_threshold;
  double m_background_median_rms;

  std::map<std::string, MetadataEntry> m_metadata;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEINFO_DETECTIONFRAMEINFO_H_ */
