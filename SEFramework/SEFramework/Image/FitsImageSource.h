/*
 * FitsImageSource.h
 *
 *  Created on: Feb 21, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_FITSIMAGESOURCE_H_
#define _SEFRAMEWORK_IMAGE_FITSIMAGESOURCE_H_

#include <iostream>
#include <type_traits>

#include <fitsio.h>
#include <iomanip>

#include "ElementsKernel/Exception.h"

#include "SEFramework/CoordinateSystem/CoordinateSystem.h"
#include "SEFramework/Image/ImageSource.h"

namespace SExtractor {

template <typename T>
class FitsImageSource : public ImageSource<T>, public std::enable_shared_from_this<ImageSource<T>>  {
public:

  FitsImageSource(const std::string &filename)
    : m_fptr(nullptr) {
    int status = 0;

    fits_open_file(&m_fptr, filename.c_str(), READONLY, &status);
    if (status != 0) {
      throw Elements::Exception() << "Can't open FITS file: " << filename;
    }
    assert(m_fptr != nullptr);

    int bitpix, naxis;
    long naxes[2] = {1,1};
    fits_get_img_param(m_fptr, 2, &bitpix, &naxis, naxes, &status);
    if (status != 0 || naxis != 2) {
      throw Elements::Exception() << "Can't find 2D image in FITS file: " << filename;
    }

    m_width = naxes[0];
    m_height = naxes[1];

//    // Read FITS keywords
//    double gain = 1, saturate = 0, flux_scale = 1;
//    readFitsKeyword("GAIN", gain);
//    readFitsKeyword("SATURATE", saturate);
//    readFitsKeyword("FLXSCALE", flux_scale);
  }

  FitsImageSource(const std::string &filename, int width, int height,
                  const std::shared_ptr<CoordinateSystem> coord_system = nullptr) : m_fptr(nullptr) {
    m_width = width;
    m_height = height;

    int status = 0;
    fits_create_file(&m_fptr, ("!"+filename).c_str(), &status);
    if (status != 0) {
      throw Elements::Exception() << "Can't create or overwrite FITS file: " << filename;
    }
    assert(m_fptr != nullptr);

    long naxes[2] = {width, height};
    fits_create_img(m_fptr, getImageType(), 2, naxes, &status);

    if (coord_system) {
      auto headers = coord_system->getFitsHeaders();
      for (auto &h : headers) {
        std::ostringstream padded_key, serializer;
        padded_key << std::setw(8) << std::left << h.first;

        serializer << padded_key.str() << "= " << std::left << std::setw(70) << h.second;
        auto str = serializer.str();

        fits_update_card(m_fptr, padded_key.str().c_str(), str.c_str(), &status);
        if (status != 0) {
          char err_txt[31];
          fits_get_errstatus(status, err_txt);
          throw Elements::Exception() << "Couldn't write the WCS headers (" << err_txt << "): " << str;
        }
      }
    }

    std::vector<T> buffer(width);
    for (int i = 0; i<height; i++) {
      long first_pixel[2] = {1, i+1};
      fits_write_pix(m_fptr, getDataType(), first_pixel, width, &buffer[0], &status);
    }
    fits_close_file(m_fptr, &status);

    if (status != 0) {
      throw Elements::Exception() << "Couldn't allocate space for new FITS file: " << filename;
    }

    m_fptr = nullptr;
    fits_open_file(&m_fptr, filename.c_str(), READWRITE, &status);
    if (status != 0) {
      throw Elements::Exception() << "Can't open FITS file for read/write: " << filename;
    }
    assert(m_fptr != nullptr);
  }

  virtual ~FitsImageSource() {
    int status = 0;
    if (m_fptr != nullptr) {
      fits_close_file(m_fptr, &status);
    }
  }

  virtual std::shared_ptr<ImageTile<T>> getImageTile(int x, int y, int width, int height) const override {
    auto tile = std::make_shared<ImageTile<T>>((const_cast<FitsImageSource*>(this))->shared_from_this(), x, y, width, height);

    long first_pixel[2] = {x+1, y+1};
    long last_pixel[2] = {x+width, y+height};
    long increment[2] = {1, 1};
    int status = 0;

    auto image = tile->getImage();
    fits_read_subset(m_fptr, getDataType(), first_pixel, last_pixel, increment,
                 nullptr, &image->getData()[0], nullptr, &status);
    if (status != 0) {
      throw Elements::Exception() << "Error reading image tile from FITS file.";
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
    auto image = tile.getImage();

    int x = tile.getPosX();
    int y = tile.getPosY();
    int width = image->getWidth();
    int height = image->getHeight();

    long first_pixel[2] = {x+1, y+1};
    long last_pixel[2] = {x+width, y+height};
    int status = 0;

    fits_write_subset(m_fptr, getDataType(), first_pixel, last_pixel, &image->getData()[0], &status);
    if (status != 0) {
      throw Elements::Exception() << "Error saving image tile to FITS file.";
    }
  }

  template <typename TT>
  bool readFitsKeyword(const std::string& header_keyword, TT& out_value) {
    double keyword_value;
    int status = 0;
    fits_read_key(m_fptr, TDOUBLE, header_keyword.c_str(), &keyword_value, nullptr, &status);
    if (status == 0) {
      out_value = keyword_value;
      return true;
    } else {
      return false;
    }
  }


private:

  int getDataType() const;
  int getImageType() const;

  int m_width;
  int m_height;

  fitsfile* m_fptr;
};

}




#endif /* _SEFRAMEWORK_IMAGE_FITSIMAGESOURCE_H_ */
