/*
 * FitsImageSource.h
 *
 *  Created on: Feb 21, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_FITSIMAGESOURCE_H_
#define _SEFRAMEWORK_IMAGE_FITSIMAGESOURCE_H_

#include <iostream>
#include "fitsio.h"
#include "SEFramework/Image/ImageSource.h"

namespace SExtractor {

template <typename T>
class FitsImageSource : public ImageSource<T>, public std::enable_shared_from_this<ImageSource<T>>  {
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

  FitsImageSource(const std::string& filename, int width, int height) : m_fptr(nullptr) {
    m_width = width;
    m_height = height;

    int status = 0;
    int bitpix, naxis;
    long naxes[2] = {width, height};

    fits_create_file(&m_fptr, ("!"+filename).c_str(), &status);
    fits_create_img(m_fptr, DOUBLE_IMG, 2, naxes, &status);

    std::vector<double> buffer(width);
    for (int i = 0; i<height; i++) {
      long first_pixel[2] = {1, i+1};
      fits_write_pix(m_fptr, TDOUBLE, first_pixel, width, &buffer[0], &status);
    }
    fits_close_file(m_fptr, &status);

    m_fptr = nullptr;
    fits_open_file(&m_fptr, filename.c_str(), READWRITE, &status);

    std::cout << "created file\n";
    assert(m_fptr != nullptr);
  }

  virtual ~FitsImageSource() {
    int status = 0;
    fits_close_file(m_fptr, &status);
  }

  virtual std::shared_ptr<ImageTile<T>> getImageTile(int x, int y, int width, int height) const override {
    auto tile = std::make_shared<ImageTile<T>>((const_cast<FitsImageSource*>(this))->shared_from_this(), x, y, width, height);

    long first_pixel[2] = {x+1, y+1};
    long last_pixel[2] = {x+width, y+height};
    long increment[2] = {1, 1};
    int status = 0;

    // FIXME
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

  virtual void saveTile(ImageTile<T>& tile) override {

    std::cout << "saving tile...\n";

    auto image = tile.getImage();

    int x = tile.getPosX();
    int y = tile.getPosY();
    int width = image->getWidth();
    int height = image->getHeight();

    long first_pixel[2] = {x+1, y+1};
    long last_pixel[2] = {x+width, y+height};
    int status = 0;

    // FIXME
    std::vector<double> buffer(width*height);
    for (int j=0; j<height; j++) {
      for (int i=0; i<width; i++) {
        buffer[i+j*width] = image->getValue(i,j);
      }
    }

    fits_write_subset(m_fptr, TDOUBLE, first_pixel, last_pixel, &buffer[0], &status);
  }


private:
  int m_width;
  int m_height;

  fitsfile* m_fptr;
};

}




#endif /* _SEFRAMEWORK_IMAGE_FITSIMAGESOURCE_H_ */
