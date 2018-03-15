/**
 * @file SEFramework/Image/FitsReader.h
 * @date 06/14/16
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_IMAGE_FITSREADER_H
#define _SEFRAMEWORK_IMAGE_FITSREADER_H

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/FitsImageSource.h"
#include "SEFramework/Image/BufferedImage.h"

namespace SExtractor {

/**
 * @class FitsReader
 * @brief
 *
 */
template <typename T>
class FitsReader {

public:

  /**
   * @brief Destructor
   */
  virtual ~FitsReader() = default;

  static std::shared_ptr<Image<T>> readFile(const std::string& filename) {
    auto image_source = std::make_shared<FitsImageSource<T>>(filename);
    return BufferedImage<T>::create(image_source);
  }

}; /* End of FitsReader class */

} /* namespace SExtractor */


#endif
