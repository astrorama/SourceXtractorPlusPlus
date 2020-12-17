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
 * ImageSource.h
 *
 *  Created on: Feb 14, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_IMAGESOURCE_H_
#define _SEFRAMEWORK_IMAGE_IMAGESOURCE_H_

#include <boost/variant.hpp>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageTile.h"

namespace SourceXtractor {

/**
 * Accepted values are bool, char, int64_t, double and string
 * Other types must be promoted (i.e. int32_t => int64_t, float => double)
 */

struct MetadataEntry {
  typedef boost::variant<bool, char, int64_t, double, std::string> value_t;

  value_t m_value;

  /// Additional metadata about the entry: i.e. comments
  std::map<std::string, std::string> m_extra;
};

/**
 * Metadata is modeled as a set of key/value pairs. Keys are unique.
 */

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

  /**
   * @return A copy of the metadata set
   */
  virtual const std::map<std::string, MetadataEntry> getMetadata() const { return {}; };

  virtual void setMetadata(std::string key, MetadataEntry value) {}

private:

};

}

#endif /* _SEFRAMEWORK_IMAGE_IMAGESOURCE_H_ */
