/*
 * FitsWriter.h
 *
 *  Created on: May 23, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_FITSWRITER_H_
#define _SEFRAMEWORK_IMAGE_FITSWRITER_H_

#include <CCfits/CCfits>
#include "SEFramework/Image/Image.h"
#include "ElementsKernel/ProgramHeaders.h"

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
    auto total = image.getHeight() * image.getWidth();
    std::valarray<double> data (total);

    //std::copy(ModelFitting::ImageTraits<T>::begin(image), ModelFitting::ImageTraits<T>::end(image), begin(data));

    for (int y = 0; y < image.getHeight(); y++) {
      for (int x = 0; x < image.getWidth(); x++) {
        data[y * image.getWidth() + x] = image.getValue(x, y);
      }
    }

    long naxis = 2;
    long naxes[2] = { image.getWidth(), image.getHeight() };
    std::unique_ptr<CCfits::FITS> pFits {new CCfits::FITS("!"+filename, DOUBLE_IMG, naxis, naxes)};
    pFits->pHDU().write(1, total, data);
    fitsWriterLogger.info() << "Created file " << filename;
  }

}; /* End of FitsReader class */

} /* namespace SExtractor */





#endif /* _SEFRAMEWORK_IMAGE_FITSWRITER_H_ */
