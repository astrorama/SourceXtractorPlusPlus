/**
 * @file Image/FitsImage.h
 * @date 05/31/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_FITSIMAGE_H
#define _SEFRAMEWORK_IMAGE_FITSIMAGE_H

#include <string>
#include <valarray>

#include "SEFramework/Image/Image.h"

namespace SExtractor {

/**
 * @class FitsImage
 * @brief
 *
 */
class FitsImage : public Image {

public:

  /**
   * @brief Destructor
   */
  virtual ~FitsImage() = default;

  FitsImage(const std::string& filename);

  virtual double getValue(int x, int y) const override;
  virtual int getWidth() const override;
  virtual int getHeight() const override;

private:
  std::valarray<unsigned long> m_data;
  int m_width;
  int m_height;

}; /* End of FitsImage class */

} /* namespace SEFramework */


#endif
