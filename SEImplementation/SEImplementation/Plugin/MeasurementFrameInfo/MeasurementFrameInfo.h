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

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEINFO_MEASUREMENTFRAMEINFO_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEINFO_MEASUREMENTFRAMEINFO_H_

#include "SEUtils/Types.h"

#include "SEFramework/Property/Property.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SourceXtractor {

class MeasurementFrameInfo : public Property {

public:
  virtual ~MeasurementFrameInfo() = default;

  MeasurementFrameInfo(int width, int height, double gain, double saturation,
      double variance_threshold, double background_median_rms)
      : m_width(width), m_height(height),
        m_gain(gain), m_saturation(saturation),
        m_variance_threshold(variance_threshold), m_background_median_rms(background_median_rms) {}

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

private:
  int m_width;
  int m_height;

  double m_gain;
  double m_saturation;

  double m_variance_threshold;
  double m_background_median_rms;

};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEINFO_MEASUREMENTFRAMEINFO_H_ */
