/**
 * @file SEFramework/Image/ImageBuffer.h
 * @date 06/03/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_IMAGEBUFFER_H
#define _SEFRAMEWORK_IMAGE_IMAGEBUFFER_H

#include <vector>

#include "Image.h"

namespace SEFramework {

/**
 * @class ImageBuffer
 * @brief
 *
 */
class ImageBuffer : public Image {
public:

  /**
   * @brief Destructor
   */
  virtual ~ImageBuffer() = default;

  ImageBuffer(int width, int height);

  virtual double getValue(int x, int y) const override;
  virtual int getWidth() const override;
  virtual int getHeight() const override;

  void setValue(int x, int y, double value) {
    m_data[x + y * m_width] = value;
  }

private:
  std::vector<double> m_data;
  int m_width;
  int m_height;

}; /* End of ImageBuffer class */

} /* namespace SEFramework */


#endif
