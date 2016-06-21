/**
 * @file SEFramework/Image/SubtractImage.h
 * @date 06/02/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_SUBTRACTIMAGE_H
#define _SEFRAMEWORK_IMAGE_SUBTRACTIMAGE_H

#include <memory>

#include "SEFramework/Image/Image.h"

namespace SExtractor {

/**
 * @class SubtractImage
 * @brief Used to subtract a constant value from an Image
 *
 */
template <typename T>
class SubtractImage : public Image<T> {

public:

  /**
   * @brief Destructor
   */
  virtual ~SubtractImage() = default;

  SubtractImage(std::shared_ptr<Image<T>> image, T value_to_subtract)
      : m_image(image), m_value_to_subtract(value_to_subtract) {};

  T getValue(int x, int y) const override {
    return m_image->getValue(x, y) - m_value_to_subtract;
  }
  
  int getWidth() const override {
    return m_image->getWidth();
  }
  
  int getHeight() const override {
    return m_image->getHeight();
  }

private:
  std::shared_ptr<Image<T>> m_image;
  T m_value_to_subtract;


}; /* End of SubtractImage class */

} /* namespace SExtractor */


#endif
