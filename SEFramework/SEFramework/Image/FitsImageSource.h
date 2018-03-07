/*
 * FitsImageSource.h
 *
 *  Created on: Feb 21, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_FITSIMAGESOURCE_H_
#define _SEFRAMEWORK_IMAGE_FITSIMAGESOURCE_H_

#include "fitsio.h"

#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/Image/FitsReader.h"

namespace SExtractor {

template <typename T>
class FitsImageSource : public ImageSource<T> {
public:

  FitsImageSource(const std::string& filename) : m_fptr(nullptr) {
    int status = 0;
    int bitpix, naxis;
    long naxes[2] = {1,1};

    fits_open_file(&m_fptr, filename.c_str(), READONLY, &status);
    fits_get_img_param(m_fptr, 2, &bitpix, &naxis, naxes, &status);

    if (naxis == 2) {
      m_width = naxes[0];
      m_height = naxes[1];
    }
  }

  virtual ~FitsImageSource() {
    int status = 0;
    fits_close_file(m_fptr, &status);
  }

  virtual std::shared_ptr<ImageTile<T>> getImageTile(int x, int y, int width, int height) const override {
    auto tile = std::make_shared<ImageTile<T>>(x, y, width, height);

    long first_pixel[2] = {x+1, y+1};
    long last_pixel[2] = {x+width, y+height};
    long increment[2] = {1, 1};
    int status = 0;

    std::vector<double> buffer(width*height);
    fits_read_subset(m_fptr, TDOUBLE, first_pixel, last_pixel, increment,
                 nullptr, &buffer[0], nullptr, &status);

    auto image = tile->getImage();
    for (int j=0; j<height; j++) {
      for (int i=0; i<width; i++) {
        image->setValue(i,j, buffer[i+j*width]);
      }
    }

    return tile;
  }

  /// Returns the width of the image in pixels
  virtual int getWidth() const override {
    return m_width;
  }

  /// Returns the height of the image in pixels
  virtual int getHeight() const  override {
    return m_height;
  }

private:
  int m_width;
  int m_height;

  fitsfile* m_fptr;
};

}




#endif /* _SEFRAMEWORK_IMAGE_FITSIMAGESOURCE_H_ */
