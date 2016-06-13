/**
 * @file src/lib/Image/FitsImage.cpp
 * @date 05/31/16
 * @author mschefer
 */

#include <CCfits/CCfits>
using namespace CCfits;

#include "SEFramework/Image/FitsImage.h"

namespace SExtractor {


FitsImage::FitsImage(const std::string& filename) {
  try {
    std::unique_ptr<FITS> pInfile(new FITS(filename, Read, true));

    PHDU& image = pInfile->pHDU();

    // read all user-specified, coordinate, and checksum keys in the image
    image.readAllKeys();
    image.read(m_data);

    m_width = image.axis(0);
    m_height = image.axis(1);

  } catch (FitsException& e) {
    // FIXME translate this exception to an appropriate Elements exception
    throw e;
  }
}

double FitsImage::getValue(int x, int y) const {
  return m_data[x + y * m_width];
}

int FitsImage::getWidth() const {
  return m_width;
}

int FitsImage::getHeight() const {
  return m_height;
}

} // SEFramework namespace



