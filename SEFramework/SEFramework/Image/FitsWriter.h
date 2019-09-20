/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*
 * FitsWriter.h
 *
 *  Created on: May 23, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_FITSWRITER_H_
#define _SEFRAMEWORK_IMAGE_FITSWRITER_H_

#include "ElementsKernel/Logging.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"
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
  static void writeFile(const Image<T> &image, const std::string &filename,
                        const std::shared_ptr<CoordinateSystem> coord_system = nullptr) {
    auto target_image = newImage<T>(filename, image.getWidth(), image.getHeight(), coord_system);

    // FIXME optimize the copy by using tile boundaries, image chunks, etc
    for (int y = 0; y < image.getHeight(); y++) {
      for (int x = 0; x < image.getWidth(); x++) {
        target_image->setValue(x, y, image.getValue(x, y));
      }
    }
  }

  template <typename T>
  static std::shared_ptr<WriteableImage<T>> newImage(const std::string &filename, int width, int height,
                                                     const std::shared_ptr<CoordinateSystem> coord_system = nullptr) {
    fitsWriterLogger.debug() << "Creating file " << filename;
    auto image_source = std::make_shared<FitsImageSource<T>>(filename, width, height, coord_system);
    return WriteableBufferedImage<T>::create(image_source);
  }

  template <typename T>
    static std::shared_ptr<WriteableImage<T>> newTemporaryImage(const std::string &pattern, int width, int height,
      bool auto_remove = true) {
    fitsWriterLogger.debug() << "Creating temporary fits file";
    auto image_source = std::make_shared<TemporaryFitsImageSource<T>>(pattern, width, height, auto_remove);
    return WriteableBufferedImage<T>::create(image_source);
  }


}; /* End of FitsReader class */

} /* namespace SExtractor */





#endif /* _SEFRAMEWORK_IMAGE_FITSWRITER_H_ */
