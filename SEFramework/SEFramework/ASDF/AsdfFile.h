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
 * AsdfFile.h
 *
 *  Created on: Aug 14, 2025
 *      Author: embray
 */

#ifndef _SEFRAMEWORK_ASDF_ASDFFILE_H_
#define _SEFRAMEWORK_ASDF_ASDFFILE_H_

#include <cstdint>

#include <boost/filesystem/path.hpp>
#include <asdf.h>

#include <ElementsKernel/Exception.h>

#include "SEFramework/Image/ImageSourceWithMetadata.h"
#include "SEFramework/Image/ImageTile.h"

namespace SourceXtractor {

/**
 * @class AsdfFile
 * @brief represents access to a whole ASDF file
 *
 */
class AsdfFile : public std::enable_shared_from_this<AsdfFile> {
public:
  /**
   * Exception thrown when trying to access a value that does not exist in the ASDF tree
   */
  class AsdfValueNotFoundException : public Elements::Exception {};

  /**
   * Exception thrown when trying to access a value of the wrong type from the ASDF tree
   */
  class AsdfValueTypeMismatchException : public Elements::Exception {};

  AsdfFile(const boost::filesystem::path& path, bool writeable);

  AsdfFile(const boost::filesystem::path& path) : AsdfFile(path, false) {}

  AsdfFile(AsdfFile&&) = default;

  virtual ~AsdfFile();

  asdf_file_t* getAsdfFilePtr();

  /**
   * Wrapper around asdf_ndarray_t
   */
  class Ndarray {
  public:
    friend class AsdfFile;

    ~Ndarray() {
      asdf_ndarray_destroy(m_ndarray_ptr);
    }

    uint32_t ndim() const { return m_ndarray_ptr->ndim; }

    std::vector<uint64_t> shape() const {
      return std::vector<uint64_t>(m_ndarray_ptr->shape,
                                   m_ndarray_ptr->shape + m_ndarray_ptr->ndim);
    }

    /**
     * Given a shared pointer to an already allocated ImageTile (i.e. from ImageTile::create)
     * copy a tile from the raw ndarray data into the ImageTile.
     */
    void fillImageTile(const std::shared_ptr<ImageTile> image_tile, int layer);

  private:
    /**
     * Private constructor for creating the `Ndarray` wrapper from a raw asdf_value_t *
     */
    explicit Ndarray(std::shared_ptr<AsdfFile> file, asdf_value_t *ptr);
    /**
     * Private constructor for creating the `Ndarray` wrapper from a raw asdf_ndarray_t *
     */
    explicit Ndarray(std::shared_ptr<AsdfFile> file, asdf_ndarray_t *ptr)
      : m_file(file), m_ndarray_ptr(ptr) {}

    std::shared_ptr<AsdfFile> m_file;
    asdf_ndarray_t* m_ndarray_ptr;
  };

  /**
   * Return the N-th ndarray from the top-level of the ASDF tree iterating the top-level
   * keys in order.
   */
  Ndarray getNdarray(int index);

  /**
   * Return any ndarray from any path in the ASDF tree
   *
   * If the given path does not exist an AsdfValueNotFoundException is thrown.
   * If the given path exists but is not an ndarray, an AsdfValueTypeMismatchException is
   * thrown.
   */
  Ndarray getNdarray(const std::string& path);

  /* TODO: More general methods for reading metadata from the ASDF tree; for the first version
   * not needed though. */

private:
  struct AsdfValueDestroy {
    void operator()(asdf_value_t *v) const noexcept {
      asdf_value_destroy(v);
    }
  };

  using AsdfValuePtr = std::unique_ptr<asdf_value_t, AsdfValueDestroy>;

  AsdfValuePtr getValue(const std::string& path);

  boost::filesystem::path m_path;
  std::unique_ptr<asdf_file_t, void (*)(asdf_file_t*)> m_asdf_ptr;

  void open();
};

}  // namespace SourceXtractor

#endif /* _SEFRAMEWORK_ASDF_ASDFFILE_H_ */
