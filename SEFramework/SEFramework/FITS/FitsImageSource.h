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

#include <memory>
#include <vector>
#include <map>

#include <boost/lexical_cast.hpp>

#include "SEFramework/CoordinateSystem/CoordinateSystem.h"
#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/FITS/FitsFileManager.h"


namespace SourceXtractor {

template <typename T>
class FitsImageSource : public ImageSource<T>, public std::enable_shared_from_this<ImageSource<T>>  {
public:

  /**
   * Constructor
   * @param filename
   *    Path to the FITS file
   * @param hdu_number
   *    HDU number. If <= 0, the constructor will use the first HDU containing an image
   * @param manager
   */
  FitsImageSource(const std::string &filename, int hdu_number = 0,
      std::shared_ptr<FitsFileManager> manager = FitsFileManager::getInstance());

  FitsImageSource(const std::string &filename, int width, int height,
                  const std::shared_ptr<CoordinateSystem> coord_system = nullptr,
                  std::shared_ptr<FitsFileManager> manager = FitsFileManager::getInstance());

  virtual ~FitsImageSource() = default;

  std::string getRepr() const override {
    std::stringstream sstream;
    sstream <<  m_filename << "[" <<  m_hdu_number << "]" << "(" << m_width << " x " << m_height << ")";
    return sstream.str();
  }

  std::shared_ptr<ImageTile<T>> getImageTile(int x, int y, int width, int height) const override;

  /// Returns the width of the image in pixels
  int getWidth() const override {
    return m_width;
  }

  /// Returns the height of the image in pixels
  int getHeight() const  override {
    return m_height;
  }

  void saveTile(ImageTile<T>& tile) override;

  template <typename TT>
  bool readFitsKeyword(const std::string& header_keyword, TT& out_value) const {
    auto i = m_header.find(header_keyword);
    if (i != m_header.end()) {
      out_value = boost::lexical_cast<TT>(i->second);
      return true;
    }
    return false;
  }

  int getHDU() const {
    return m_hdu_number;
  }

  std::unique_ptr<std::vector<char>> getFitsHeaders(int& number_of_records) const;

private:

  void switchHdu(fitsfile* fptr, int hdu_number) const;

  std::map<std::string, std::string> loadFitsHeader(fitsfile *fptr);
  void loadHeadFile();

  int getDataType() const;
  int getImageType() const;

  std::string m_filename;
  std::shared_ptr<FitsFileManager> m_manager;

  int m_width;
  int m_height;

  int m_hdu_number;

  std::map<std::string, std::string> m_header;
};

}




#endif /* _SEFRAMEWORK_IMAGE_FITSIMAGESOURCE_H_ */
