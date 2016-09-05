/**
 * @file SEFramework/Image/Image.h
 * @date 05/30/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_IMAGE_H
#define _SEFRAMEWORK_IMAGE_IMAGE_H

#include <memory>

#include "SEUtils/PixelCoordinate.h"

namespace SExtractor {

/**
 * @class Image
 * @brief Interface representing an image
 * @tparam T the type of the pixel values
 */
template <typename T>
class Image {
  
public:
  
  using PixelType = T;

  /**
   * @brief Destructor
   */
  virtual ~Image() = default;

  /// Returns the value of the pixel with the coordinates (x,y)
  virtual T getValue(int x, int y) const = 0;
  
  T getValue(PixelCoordinate pc) const {
    return getValue(pc.m_x, pc.m_y);
  }

  /// Returns the width of the image in pixels
  virtual int getWidth() const = 0;
  
  /// Returns the height of the image in pixels
  virtual int getHeight() const = 0;

}; /* End of Image class */

/// Alias for the detection image, to make easier its type modification
using DetectionImage = Image<float>;

/// Alias for the flag image, to make easier its type modification
using FlagImage = Image<std::int64_t>;

} /* namespace SExtractor */


#endif
