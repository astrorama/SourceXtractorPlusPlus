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
 * @file SEImplementation/Property/DetectionFramePixelValues.h
 * @date 05/27/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAMEPIXELVALUES_H
#define _SEIMPLEMENTATION_PROPERTY_DETECTIONFRAMEPIXELVALUES_H

#include <vector>

#include "SEFramework/Property/Property.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

/**
 * @class DetectionFramePixelValues
 * @brief The values of a Source's pixels in the detection image. They are returned as
 * a vector in the same order as the Source's pixels.
 *
 */
class DetectionFramePixelValues : public Property {
public:

  /**
   * @brief Destructor
   */
  virtual ~DetectionFramePixelValues() = default;

  DetectionFramePixelValues(
      std::vector<DetectionImage::PixelType> values,
      std::vector<DetectionImage::PixelType> filtered_values,
      std::vector<WeightImage::PixelType> variances)
    : m_values(std::move(values)), m_filtered_values(filtered_values), m_variances(variances) {}

  const std::vector<DetectionImage::PixelType>& getValues() const {
    return m_values;
  }

  const std::vector<DetectionImage::PixelType>& getFilteredValues() const {
    return m_filtered_values;
  }

  const std::vector<DetectionImage::PixelType>& getVariances() const {
    return m_variances;
  }

private:

  std::vector<DetectionImage::PixelType> m_values;
  std::vector<DetectionImage::PixelType> m_filtered_values;
  std::vector<DetectionImage::PixelType> m_variances;

}; /* End of DetectionFramePixelValues class */

} /* namespace SExtractor */


#endif
