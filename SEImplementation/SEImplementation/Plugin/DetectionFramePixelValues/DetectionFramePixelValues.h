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

  DetectionFramePixelValues(std::vector<DetectionImage::PixelType> values) : m_values(std::move(values)) {}

  const std::vector<DetectionImage::PixelType>& getValues() const {
    return m_values;
  }

private:

  std::vector<DetectionImage::PixelType> m_values;

}; /* End of DetectionFramePixelValues class */

} /* namespace SExtractor */


#endif
