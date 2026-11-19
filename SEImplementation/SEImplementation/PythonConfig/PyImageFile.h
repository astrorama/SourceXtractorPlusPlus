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
 * PyImageFile.h
 *
 * An image file of any supported format, as seen from a configuration script.
 */

#ifndef _SEIMPLEMENTATION_PYTHONCONFIG_PYIMAGEFILE_H
#define _SEIMPLEMENTATION_PYTHONCONFIG_PYIMAGEFILE_H

#include <string>
#include <vector>

#include "SEFramework/Image/Dimensions.h"
#include "SEFramework/Image/ImageSource.h"


namespace SourceXtractor {
/**
 * An image file opened by SourceXtractor++
 *
 * The format-agnostic counterpart of PyFitsFile, images within the file are
 * addressed the way the rest of SE++ addresses them: by index, or by a name
 * whose meaning is the format's own, e.g. an ASDF tree path, or a FITS EXTNAME.
 */
class PyImageFile {

public:

  explicit PyImageFile(const std::string& filename);

  virtual ~PyImageFile() = default;

  std::string getFilename() const {
    return m_filename;
  }

  /// The name of the file's format, e.g. "fits" or "asdf"
  std::string getFormat() const;

  /**
   * The names by which the images in this file can be addressed, in file order
   *
   * An image with no name of its own gets an empty entry.  For a FITS file the
   * entries line up with image_hdus.
   */
  std::vector<std::string> getImageNames() const;

  /// The metadata of one image in the file
  std::shared_ptr<const Metadata> getMetadata(const std::string& name) const;

  /// As getMetadata(), for the nth image in the file
  virtual std::shared_ptr<const Metadata> getMetadataByIndex(int index) const;

  /**
   * The dimensions of one image in the file, slowest axis first
   *
   * An ImageSource reports width and height but not how many layers a data
   * cube has, so a cube cannot be sized through this interface (FIXME).
   */
  Dimensions getDimensions(const std::string& name) const;

  /// As getDimensions(), for the nth image in the file
  virtual Dimensions getDimensionsByIndex(int index) const;

protected:
  /// Opens the file afresh; readers are cheap and the file pool does the caching
  std::shared_ptr<ImageSource> openImage(const std::string& name) const;
  std::shared_ptr<ImageSource> openImage(int index) const;

private:
  std::string m_filename;
};

}  // namespace SourceXtractor

#endif /* _SEIMPLEMENTATION_PYTHONCONFIG_PYIMAGEFILE_H */
