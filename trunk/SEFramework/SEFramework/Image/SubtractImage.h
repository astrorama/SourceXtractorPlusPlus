/**
 * @file SEFramework/Image/SubtractImage.h
 * @date 06/02/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_SUBTRACTIMAGE_H
#define _SEFRAMEWORK_IMAGE_SUBTRACTIMAGE_H

#include <memory>

#include "Image.h"

namespace SEFramework {

/**
 * @class SubtractImage
 * @brief
 *
 */
class SubtractImage : public Image {

public:

  /**
   * @brief Destructor
   */
  virtual ~SubtractImage() = default;

  SubtractImage(std::shared_ptr<Image> image, double value_to_subtract)
      : m_image(image), m_value_to_subtract(value_to_subtract) {};

  virtual double getValue(int x, int y) const override;
  virtual int getWidth() const override;
  virtual int getHeight() const override;

private:
  std::shared_ptr<Image> m_image;
  double m_value_to_subtract;


}; /* End of SubtractImage class */

} /* namespace SEFramework */


#endif
