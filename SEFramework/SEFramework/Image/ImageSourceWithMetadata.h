/** Copyright © 2020 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#ifndef _SEFRAMEWORK_IMAGE_IMAGESOURCEWITHMETADATA_H_
#define _SEFRAMEWORK_IMAGE_IMAGESOURCEWITHMETADATA_H_

#include <boost/variant.hpp>
#include "SEFramework/Image/ImageSource.h"

namespace SourceXtractor {

/**
 * ImageSource with additional metadata: i.e. coming from FITS headers
 * Metadata is modeled as a set of key/value pairs. Keys are unique.
 */
template <typename T>
class ImageSourceWithMetadata: public ImageSource<T> {
public:
  /**
   * Accepted values are bool, int64_t, double and string
   * Other types must be promoted (i.e. int32_t => int64_t, float => double)
   */
  struct MetadataEntry {
    typedef boost::variant<bool, int64_t, double, std::string> value_t;

    value_t m_value;

    /// Additional metadata about the entry: i.e. comments
    std::map<std::string, std::string> m_extra;
  };

  virtual ~ImageSourceWithMetadata() = default;

  /**
   * @return A copy of the metadata set
   */
  virtual std::map<std::string, MetadataEntry> getMetadata() const = 0;
};

} // end of namespace SourceXtractor

#endif /* _SEFRAMEWORK_IMAGE_IMAGESOURCEWITHMETADATA_H_ */
