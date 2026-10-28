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
 * ImageSource.h
 *
 *  Created on: Feb 14, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_IMAGESOURCE_H_
#define _SEFRAMEWORK_IMAGE_IMAGESOURCE_H_

#include "SEFramework/CoordinateSystem/CoordinateSystem.h"
#include "SEFramework/Image/ImageTile.h"
#include "SEFramework/Image/Metadata.h"

#include <memory>


namespace SourceXtractor {

class ImageSource {
public:

  ImageSource() {}

  virtual ~ImageSource() = default;

  /// Human readable representation of this source
  virtual std::string getRepr() const = 0;

  virtual void saveTile(ImageTile& tile) = 0;
  virtual std::shared_ptr<ImageTile> getImageTile(int x, int y, int width, int height) const = 0;


  /// Returns the width of the image in pixels
  virtual int getWidth() const = 0;

  /// Returns the height of the image in pixels
  virtual int getHeight() const = 0;

  /// Sets the current layer to use if the image is in a data cube
  virtual void setLayer(int /* unused */) { }

  virtual ImageTile::ImageType getType() const = 0;

  /**
   * The coordinate system of this image, or nullptr if it has none
   *
   * The returned coordinate system is a fresh instance the caller owns; it is
   * not shared between calls, since WCS is mutable.
   *
   * @return The coordinate system, or nullptr if the image carries no usable one
   */
  virtual std::shared_ptr<CoordinateSystem> getCoordinateSystem() const { return nullptr; }

  /**
   * The metadata this image carries, if any
   *
   * Never returns NULL but may return an empty Metadata if the source carries
   * no metadata of its own.
   */
  virtual std::shared_ptr<const Metadata> getMetadata() const {
    return m_metadata ? std::static_pointer_cast<const Metadata>(m_metadata) : Metadata::empty();
  }

  /// Add or replace one metadata value
  virtual void setMetadata(const std::string& key, const MetadataEntry& value) {
    if (!m_metadata) {
      m_metadata = std::make_shared<MapMetadata>();
    }

    m_metadata->set(key, value);
  }

private:
  /// Built on first write; a source never written to shares Metadata::empty()
  std::shared_ptr<MapMetadata> m_metadata;
};

}  // namespace SourceXtractor

#endif /* _SEFRAMEWORK_IMAGE_IMAGESOURCE_H_ */
