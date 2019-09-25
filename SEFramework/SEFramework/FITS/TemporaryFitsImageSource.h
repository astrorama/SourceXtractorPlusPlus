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
 * TemporaryFitsImageSource.h
 *
 *  Created on: May 23, 2018
 *      Author: aalvarez
 */

#ifndef _SEFRAMEWORK_IMAGE_TEMPORARYFITSIMAGESOURCE_H_
#define _SEFRAMEWORK_IMAGE_TEMPORARYFITSIMAGESOURCE_H_

#include "FitsImageSource.h"
#include "SEFramework/Filesystem/TemporaryFile.h"

namespace SExtractor {

/**
 * @brief FitsTemporaryImageSource wraps FitsImageSource, generating the filename and acquiring its ownership
 */
template <typename T>
class TemporaryFitsImageSource : public ImageSource<T>, public std::enable_shared_from_this<ImageSource<T>>  {
public:
  TemporaryFitsImageSource(const std::string &dir, const std::string &pattern,
    int width, int height, bool autoremove = true)
      : m_temp_file(dir, pattern, autoremove),
        m_image_source(new FitsImageSource<T>(m_temp_file.getPath(), width, height)) {
  }

  TemporaryFitsImageSource(const std::string pattern, int width, int height, bool autoremove = true)
      : TemporaryFitsImageSource("", pattern, width, height, autoremove) {
  }

  TemporaryFitsImageSource(int width, int height, bool autoremove = true)
      : TemporaryFitsImageSource("", "", width, height, autoremove) {
  }

  virtual ~TemporaryFitsImageSource() = default;

  std::string getFullPath() const {
    return m_temp_file.getPath();
  }

  virtual std::string getRepr() const override {
    return getFullPath();
  }

  virtual std::shared_ptr<ImageTile<T>> getImageTile(int x, int y, int width, int height) const override {
    return m_image_source->getImageTile(x, y, width, height);
  }

  virtual void saveTile(ImageTile<T>& tile) override {
    return m_image_source->saveTile(tile);
  }

  virtual int getWidth() const override {
    return m_image_source->getWidth();
  }

  virtual int getHeight() const override {
    return m_image_source->getHeight();
  }

private:
  TemporaryFile m_temp_file;
  std::shared_ptr<FitsImageSource<T>> m_image_source;
};

}

#endif //_SEFRAMEWORK_IMAGE_TEMPORARYFITSIMAGESOURCE_H_
