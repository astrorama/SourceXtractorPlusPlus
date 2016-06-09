/**
 * @file src/lib/Image/SubtractImage.cpp
 * @date 06/02/16
 * @author mschefer
 */

#include "SEFramework/Image/SubtractImage.h"

namespace SEFramework {

double SubtractImage::getValue(int x, int y) const {
  return m_image->getValue(x, y) - m_value_to_subtract;
}

int SubtractImage::getWidth() const {
  return m_image->getWidth();
}

int SubtractImage::getHeight() const {
  return m_image->getHeight();
}

} // SEFramework namespace



