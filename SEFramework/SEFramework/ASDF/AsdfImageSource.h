/**
 * Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
 * AsdfImageSource.h
 *
 *  Created on: Sep 03, 2025
 *      Author: embray
 */

#ifndef _SEFRAMEWORK_IMAGE_ASDFIMAGESOURCE_H_
#define _SEFRAMEWORK_IMAGE_ASDFIMAGESOURCE_H_

#include <ElementsKernel/Exception.h>
#include <FilePool/FileManager.h>

#include "SEFramework/ASDF/AsdfFile.h"
#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/Image/ImageTile.h"


namespace SourceXtractor {

using Euclid::FilePool::FileManager;
using Euclid::FilePool::FileHandler;

/**
 * Read images from ASDF files
 *
 * Similar to FitsImageSource but simpler currently as it only allows reading image tiles from
 * ASDF, not writing, nor does it support metadata lookup (yet).  These features will come later.
 */
class AsdfImageSource : public ImageSource, public std::enable_shared_from_this<ImageSource> {
public:

  /**
   * Exception raised when instantiating an AsdfImageSource with an ndarray path that is
   * not a supported image type (not 2- or 3-D, and not a supported ImageTile::ImageType
   */
  class InvalidImageNdarrayException : public Elements::Exception {};

  /**
   * Constructor
   * @param filename
   *    Path to the ASDF file
   * @param image_index
   *    An integer index of the the ndarray to load the image from.  When using a numerical
   *    index, only ndarrays in the top-level of the ASDF tree are considered in the enumeration,
   *    with ndarrays containing non-image data (i.e. not 2 or 3 dimensions, or containing a
   *    non-scalar datatype) skipped over.  By default the first image ndarray found in the
   *    file is loaded.
   */
  explicit AsdfImageSource(const std::string& filename, int image_index = 0,
                           ImageTile::ImageType image_type = ImageTile::AutoType,
                           std::shared_ptr<FileManager> manager = FileManager::getDefault()) :
    AsdfImageSource(filename, image_index, std::nullopt, image_type, std::move(manager)) {}

  /**
   * Constructor
   * @param filename
   *    Path to the ASDF file
   * @param image_path
   *    JSON Path into the tree of the image ndarray to load.  This can be any arbitrarily
   *    nested ndarray.  ndarrays in the root of the tree can be addressed just like "key"
   *    or "/key" though the leading slash may be omitted.  If the given path is not for a
   *    valid image ndarray an AsdfFile::AsdfValueTypeMismatchException is thrown.
   */
  explicit AsdfImageSource(const std::string& filename, const std::string& image_path,
                           ImageTile::ImageType image_type = ImageTile::AutoType,
                           std::shared_ptr<FileManager> manager = FileManager::getDefault()) :
    AsdfImageSource(filename, 0, image_path, image_type, std::move(manager)) {}

  std::shared_ptr<ImageTile> getImageTile(int x, int y, int width, int height) const override;

  std::string getRepr() const override {
    return m_filename;
  }

  int getNdim() const {
    return m_ndarray->ndim();
  }

  /**
   * Returns the width of the image in pixels
   */
  int getWidth() const override {
    return m_ndarray->shape().at(m_ndarray->ndim() - 2);
  }

  /**
   * Returns the height of the image in pixels
   */
  int getHeight() const override {
    return m_ndarray->shape().at(m_ndarray->ndim() - 1);
  }

  /**
   * Returns the height of the image in pixels
   */
  int getDepth() const {
    return m_ndarray->ndim() == 3 ? m_ndarray->shape().at(0) : 0;
  }

  /**
   * Returns the data type of the pixel values
   *
   * NOTE: Does not necessarily return the data type of the raw array, but
   * rather the type specified when constructing the AsdfImageSource, to which
   * the original data is converted.
   */
  ImageTile::ImageType getType() const override {
    return m_image_type;
  }

  void setLayer(int layer) override;

  void saveTile(ImageTile& tile) override;

  /** Get optional FITS WCS */
  std::unique_ptr<AsdfFile::FitsWCS> getFitsWCS() const;

  /** Get optional FITS WCS with optional path lookup map */
  std::unique_ptr<AsdfFile::FitsWCS> getFitsWCS(std::optional<std::string> wcs_path) const;

private:
  AsdfImageSource(const std::string& filename, int image_index,
                  std::optional<std::string> image_path,
                  ImageTile::ImageType image_type,
                  std::shared_ptr<FileManager> manager);

  using WcsPathMap = std::unordered_map<std::string, std::string>;

  WcsPathMap parseWcsPath(const std::string& wcs_path) const;

  std::string m_filename;
  ImageTile::ImageType m_image_type;
  std::shared_ptr<FileManager> m_file_manager;
  std::shared_ptr<FileHandler> m_handler;

  std::string m_image_path;

  std::shared_ptr<AsdfFile::Ndarray> m_ndarray;
  int m_current_layer;
};
}

#endif /* _SEFRAMEWORK_IMAGE_ASDFIMAGESOURCE_H_ */
