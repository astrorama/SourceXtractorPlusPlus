/*
 * FitsWriter.h
 *
 *  Created on: May 23, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_FITSWRITER_H_
#define _SEFRAMEWORK_IMAGE_FITSWRITER_H_

#include "ElementsKernel/Logging.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/FitsImageSource.h"
#include "SEFramework/Image/TemporaryFitsImageSource.h"
#include "SEFramework/Image/WriteableBufferedImage.h"

namespace SExtractor {

static Elements::Logging fitsWriterLogger = Elements::Logging::getLogger("FitsWriter");

/**
 * @class FitsWriter
 * @brief
 *
 */
class FitsWriter {

public:

  /**
   * @brief Destructor
   */
  virtual ~FitsWriter() = default;

  template <typename T>
  static void writeFile(const Image<T>& image, const std::string& filename) {
    auto target_image = newImage<T>(filename, image.getWidth(), image.getHeight());

    // FIXME optimize the copy by using tile boundaries, image chunks, etc
    for (int y = 0; y < image.getHeight(); y++) {
      for (int x = 0; x < image.getWidth(); x++) {
        target_image->setValue(x, y, image.getValue(x, y));
      }
    }
  }

  template <typename T>
  static std::shared_ptr<WriteableImage<T>> newImage(const std::string& filename, int width, int height) {
    std::cout << "Creating file " << filename << '\n';
    auto image_source = std::make_shared<FitsImageSource<T>>(filename, width, height);
    return WriteableBufferedImage<T>::create(image_source);
  }

  template <typename T>
    static std::shared_ptr<WriteableImage<T>> newTemporaryImage(const std::string &pattern, int width, int height,
      bool auto_remove = true) {
    std::cout << "Creating temporary fits file \n";
    auto image_source = std::make_shared<TemporaryFitsImageSource<T>>(pattern, width, height, auto_remove);
    return WriteableBufferedImage<T>::create(image_source);
  }


}; /* End of FitsReader class */

} /* namespace SExtractor */





#endif /* _SEFRAMEWORK_IMAGE_FITSWRITER_H_ */
