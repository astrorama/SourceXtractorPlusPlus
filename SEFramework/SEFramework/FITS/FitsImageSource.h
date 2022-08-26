/**
 * Copyright © 2019-2022 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include "FilePool/FileManager.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"
#include "SEFramework/Image/ImageSourceWithMetadata.h"
#include "SEFramework/FITS/FitsFile.h"
#include "SEUtils/VariantCast.h"


namespace SourceXtractor {

using Euclid::FilePool::FileManager;
using Euclid::FilePool::FileHandler;

class FitsImageSource : public ImageSource, public std::enable_shared_from_this<ImageSource> {
public:


  /**
   * Constructor
   * @param filename
   *    Path to the FITS file
   * @param hdu_number
   *    HDU number. If <= 0, the constructor will use the first HDU containing an image
   * @param manager
   */
  explicit FitsImageSource(const std::string& filename, int hdu_number = 0,
                  ImageTile::ImageType image_type = ImageTile::AutoType,
                  std::shared_ptr<FileManager> manager = FileManager::getDefault());

  FitsImageSource(const std::string& filename, int width, int height,
                  ImageTile::ImageType image_type,
                  const std::shared_ptr<CoordinateSystem> coord_system = nullptr,
                  bool append = false,
                  bool empty_primary = false,
                  std::shared_ptr<FileManager> manager = FileManager::getDefault());

  virtual ~FitsImageSource() = default;

  std::string getRepr() const override {
    return m_filename;
  }

  /// Returns the width of the image in pixels
  int getWidth() const override {
    return m_width;
  }

  /// Returns the height of the image in pixels
  int getHeight() const override {
    return m_height;
  }

  /// Returns the depth of the image in pixels
  int getDepth() const {
    return m_depth;
  }

  void setLayer(int layer);

  std::shared_ptr<ImageTile> getImageTile(int x, int y, int width, int height) const override;

  void saveTile(ImageTile& tile) override;

  template<typename TT>
  bool readFitsKeyword(const std::string& header_keyword, TT& out_value) const {
    auto& headers = getMetadata();
    auto i = headers.find(header_keyword);
    if (i != headers.end()) {
      out_value = VariantCast<TT>(i->second.m_value);
      return true;
    }
    return false;
  }

  int getHDU() const {
    return m_hdu_number;
  }

  ImageTile::ImageType getType() const override {
    return m_image_type;
  }

  std::unique_ptr<std::vector<char>> getFitsHeaders(int& number_of_records) const;

  const std::map<std::string, MetadataEntry> getMetadata() const override;

  void setMetadata(std::string key, MetadataEntry value) override;

private:
  void switchHdu(fitsfile *fptr, int hdu_number) const;

  int getDataType() const;

  int getImageType() const;

  std::string m_filename;
  std::shared_ptr<FileManager> m_file_manager;
  std::shared_ptr<FileHandler> m_handler;

  int m_hdu_number;

  int m_width;
  int m_height;
  int m_depth;
  ImageTile::ImageType m_image_type;

  int m_current_layer;
};

}


#endif /* _SEFRAMEWORK_IMAGE_FITSIMAGESOURCE_H_ */
