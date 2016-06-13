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
 * @brief
 *
 */
class Image {
public:

  /**
   * @brief Destructor
   */
  virtual ~Image() = default;

  virtual double getValue(int x, int y) const = 0;
  virtual int getWidth() const = 0;
  virtual int getHeight() const = 0;

  virtual std::shared_ptr<Image> getSubImage(PixelCoordinate min_coord, PixelCoordinate max_coord) const;


private:

}; /* End of Image class */

} /* namespace SEFramework */


#endif
