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
#include <asdf/gwcs/gwcs.h>

#include <ElementsKernel/Exception.h>

#include "SEFramework/Image/ImageSourceWithMetadata.h"
#include "SEFramework/Image/ImageTile.h"

namespace SourceXtractor {

/**
 * @class AsdfFile
 * @brief represents access to a whole ASDF file
 *
 */
class AsdfFile {
public:
  /**
   * Exception thrown when trying to access a value that does not exist in the ASDF tree
   */
  class AsdfValueNotFoundException : public Elements::Exception {};

  /**
   * Exception thrown when trying to access a value of the wrong type from the ASDF tree
   */
  class AsdfValueTypeMismatchException : public Elements::Exception {};

  /**
   * Exception thrown when an ASDF ndarray contains a datatype not (currently) supported by
   * SE++
   */
  class AsdfUnsupportedDatatypeException: public Elements::Exception {};

  AsdfFile(const boost::filesystem::path& path, bool writeable);

  AsdfFile(const boost::filesystem::path& path) : AsdfFile(path, false) {}

  AsdfFile(AsdfFile&&) = default;

  virtual ~AsdfFile();

  asdf_file_t* getAsdfFilePtr() const { return m_asdf_ptr.get(); }

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

    std::vector<uint64_t>& shape() const {
      if (!m_shape) {
        m_shape = std::make_unique<std::vector<uint64_t>>(
          m_ndarray_ptr->shape,
          m_ndarray_ptr->shape + m_ndarray_ptr->ndim
        );
      }
      return *m_shape;
    }

    ImageTile::ImageType getImageType() const { return m_image_type; }

    /**
     * Convenience method to test whether the ndarray can be read as an image by SE++
     *
     * That is, it has ndim == 2 || ndim == 3 (for data cubes) and a supported datatype.
     */
    bool isSupportedImage() const;

    /**
     * Given a shared pointer to an already allocated ImageTile (i.e. from ImageTile::create)
     * copy a tile from the raw ndarray data into the ImageTile.
     */
    void fillImageTile(const std::shared_ptr<ImageTile> image_tile, int layer);

    const std::string& getPath() const { return m_path; }

  private:
    /**
     * Private constructor for creating the `Ndarray` wrapper from a raw asdf_value_t *
     */
    explicit Ndarray(const AsdfFile& file, asdf_value_t *ptr);
    /**
     * Private constructor for creating the `Ndarray` wrapper from a raw asdf_ndarray_t *
     */
    explicit Ndarray(asdf_ndarray_t *ptr)
      : m_ndarray_ptr(ptr) {}

    asdf_ndarray_t* m_ndarray_ptr;
    // Store the JSON Path to the ndarray
    std::string m_path;
    ImageTile::ImageType m_image_type;
    mutable std::unique_ptr<std::vector<uint64_t>> m_shape;
  };

  /**
   * Wrapper around asdf_gwcs_fits_t, which represents a FITS- (WCSLIB)
   * compatible WCS for use with SourceXtractor::WCS
   */
  class FitsWCS {
  public:
    friend class AsdfFile;

    ~FitsWCS() {
      asdf_gwcs_fits_destroy(m_gwcs_fits_ptr);
    }

    std::array<double, 2> crpix() const noexcept {
      return {m_gwcs_fits_ptr->crpix[0], m_gwcs_fits_ptr->crpix[1]};
    }

    std::array<double, 2> crval() const noexcept {
      return {m_gwcs_fits_ptr->crval[0], m_gwcs_fits_ptr->crval[1]};
    }

    std::array<double, 2> cdelt() const noexcept {
      return {m_gwcs_fits_ptr->cdelt[0], m_gwcs_fits_ptr->cdelt[1]};
    }

    std::array<std::array<double, 2>, 2> pc() const noexcept {
      return {{
        {m_gwcs_fits_ptr->pc[0][0], m_gwcs_fits_ptr->pc[0][1]},
        {m_gwcs_fits_ptr->pc[1][0], m_gwcs_fits_ptr->pc[1][1]}
      }};
    }

    std::array<std::string_view, 2> ctype() const noexcept {
      return {{
        m_gwcs_fits_ptr->ctype[0] ? std::string_view(m_gwcs_fits_ptr->ctype[0]) : std::string_view(),
        m_gwcs_fits_ptr->ctype[1] ? std::string_view(m_gwcs_fits_ptr->ctype[1]) : std::string_view()
      }};
    }

  private:
    /**
     * Private constructor for creating the `Ndarray` wrapper from a raw asdf_value_t *
     */
    explicit FitsWCS(const AsdfFile& file, asdf_value_t *ptr);
    /**
     * Private constructor for creating the `Ndarray` wrapper from a raw asdf_ndarray_t *
     */
    explicit FitsWCS(asdf_gwcs_fits_t *ptr)
      : m_gwcs_fits_ptr(ptr) {}

    asdf_gwcs_fits_t* m_gwcs_fits_ptr;
  };

  /**
   * Return the N-th ndarray from the top-level of the ASDF tree iterating the top-level
   * keys in order.
   */
  std::unique_ptr<Ndarray> getNdarray(int index);

  /**
   * Return any ndarray from any path in the ASDF tree
   *
   * If the given path does not exist an AsdfValueNotFoundException is thrown.
   * If the given path exists but is not an ndarray, an AsdfValueTypeMismatchException is
   * thrown.
   */
  std::unique_ptr<Ndarray> getNdarray(const std::string& path);

  /* TODO: More general methods for reading metadata from the ASDF tree; for the first version
   * not needed though. */
  /**
   * Return FITS-compatible WCS metadata (wrapped in `AsdfFile::FitsWCS`)
   *
   * If called without any arguments it will look for the first applicable GWCS
   * object in the ASDF metadata tree.  Called with a path argument it will
   * look for one specifically at that path.  In either case if no matching
   * GWCS is found will return a null-ish pointer.
   */
  std::unique_ptr<FitsWCS> getFitsWCS();
  std::unique_ptr<FitsWCS> getFitsWCS(const std::string& path);

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
