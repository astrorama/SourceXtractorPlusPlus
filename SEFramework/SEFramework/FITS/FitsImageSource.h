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

#include "SEFramework/FITS/FitsFileManager.h"


namespace SExtractor {

template <typename T>
class FitsImageSource : public ImageSource<T>, public std::enable_shared_from_this<ImageSource<T>>  {
public:

  FitsImageSource(const std::string &filename, int hdu_number = 1,
      std::shared_ptr<FitsFileManager> manager = FitsFileManager::getInstance())
    : m_filename(filename), m_manager(manager), m_hdu_number(hdu_number) {

    auto fptr = m_manager->getFitsFile(filename);
    switchHdu(fptr, m_hdu_number);

    int status = 0;
    int bitpix, naxis;
    long naxes[2] = {1,1};
    fits_get_img_param(fptr, 2, &bitpix, &naxis, naxes, &status);
    if (status != 0 || naxis != 2) {
      throw Elements::Exception() << "Can't find 2D image in FITS file: " << filename;
    }

    m_width = naxes[0];
    m_height = naxes[1];
  }

  FitsImageSource(const std::string &filename, int width, int height,
                  const std::shared_ptr<CoordinateSystem> coord_system = nullptr,
                  std::shared_ptr<FitsFileManager> manager = FitsFileManager::getInstance())
    : m_filename(filename), m_manager(manager), m_hdu_number(1) {
    m_width = width;
    m_height = height;

    {
      // CREATE NEW FITS FILE
      int status = 0;
      fitsfile* fptr = nullptr;
      fits_create_file(&fptr, ("!"+filename).c_str(), &status);
      if (status != 0) {
        throw Elements::Exception() << "Can't create or overwrite FITS file: " << filename;
      }
      assert(fptr != nullptr);

      long naxes[2] = {width, height};
      fits_create_img(fptr, getImageType(), 2, naxes, &status);

      if (coord_system) {
        auto headers = coord_system->getFitsHeaders();
        for (auto& h : headers) {
          std::ostringstream padded_key, serializer;
          padded_key << std::setw(8) << std::left << h.first;

          serializer << padded_key.str() << "= " << std::left << std::setw(70) << h.second;
          auto str = serializer.str();

          fits_update_card(fptr, padded_key.str().c_str(), str.c_str(), &status);
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
        fits_write_pix(fptr, getDataType(), first_pixel, width, &buffer[0], &status);
      }
      fits_close_file(fptr, &status);

      if (status != 0) {
        throw Elements::Exception() << "Couldn't allocate space for new FITS file: " << filename;
      }
    }

    auto fptr = m_manager->getFitsFile(filename, true);
    switchHdu(fptr, m_hdu_number);
  }

  virtual ~FitsImageSource() {
  }

  virtual std::string getRepr() const override {
    return m_filename;
  }

  virtual std::shared_ptr<ImageTile<T>> getImageTile(int x, int y, int width, int height) const override {
    auto fptr = m_manager->getFitsFile(m_filename);
    switchHdu(fptr, m_hdu_number);

    auto tile = std::make_shared<ImageTile<T>>((const_cast<FitsImageSource*>(this))->shared_from_this(), x, y, width, height);

    long first_pixel[2] = {x+1, y+1};
    long last_pixel[2] = {x+width, y+height};
    long increment[2] = {1, 1};
    int status = 0;

    auto image = tile->getImage();
    fits_read_subset(fptr, getDataType(), first_pixel, last_pixel, increment,
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
    auto fptr = m_manager->getFitsFile(m_filename, true);
    switchHdu(fptr, m_hdu_number);

    auto image = tile.getImage();

    int x = tile.getPosX();
    int y = tile.getPosY();
    int width = image->getWidth();
    int height = image->getHeight();

    long first_pixel[2] = {x+1, y+1};
    long last_pixel[2] = {x+width, y+height};
    int status = 0;

    fits_write_subset(fptr, getDataType(), first_pixel, last_pixel, &image->getData()[0], &status);
    if (status != 0) {
      throw Elements::Exception() << "Error saving image tile to FITS file.";
    }
  }

  template <typename TT>
  bool readFitsKeyword(const std::string& header_keyword, TT& out_value) {
    auto fptr = m_manager->getFitsFile(m_filename);
    switchHdu(fptr, m_hdu_number);

    double keyword_value;
    int status = 0;
    fits_read_key(fptr, TDOUBLE, header_keyword.c_str(), &keyword_value, nullptr, &status);
    if (status == 0) {
      out_value = keyword_value;
      return true;
    } else {
      return false;
    }
  }


private:

  void switchHdu(fitsfile* fptr, int hdu_number) const {
    int status = 0;
    int hdu_type = 0;

    fits_movabs_hdu(fptr, hdu_number, &hdu_type, &status);

    if (status != 0) {
      throw Elements::Exception() << "Could not switch to HDU # " << hdu_number << " in file " << m_filename;
    }
    if (hdu_type != IMAGE_HDU) {
      throw Elements::Exception() << "Trying to access non-image HDU in file " << m_filename;
    }
  }

  int getDataType() const;
  int getImageType() const;

  std::string m_filename;
  std::shared_ptr<FitsFileManager> m_manager;

  int m_width;
  int m_height;

  int m_hdu_number;
};

}




#endif /* _SEFRAMEWORK_IMAGE_FITSIMAGESOURCE_H_ */
