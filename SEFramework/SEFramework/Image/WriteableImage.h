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
 * WriteableImage.h
 *
 *  Created on: Mar 8, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_WRITEABLEIMAGE_H_
#define _SEFRAMEWORK_IMAGE_WRITEABLEIMAGE_H_

#include "SEFramework/Image/Image.h"
#include <mutex>

namespace SourceXtractor {

template <typename T>
class WriteableImage : public virtual Image<T> {
public:

  /**
   * Keep the current tile available for a series of writes. Writes take effect immediately.
   * The image must outlive the scope. Callers must serialize access to the same writable
   * image and synchronize overlapping pixel reads, including reads through retained chunks.
   */
  class WriteBatch {
  public:
    explicit WriteBatch(WriteableImage& image) : m_image(image) {
      m_image.beginWriteBatch();
    }

    ~WriteBatch() {
      m_image.endWriteBatch();
    }

    WriteBatch(const WriteBatch&) = delete;
    WriteBatch& operator=(const WriteBatch&) = delete;

  private:
    WriteableImage& m_image;
  };

  virtual void setValue(int x, int y, T value) = 0;

  // This mutex can be used to lock the writeable image if used multithreaded
  std::mutex m_write_mutex;

private:
  virtual void beginWriteBatch() noexcept {}
  virtual void endWriteBatch() noexcept {}
};

}

#endif /* _SEFRAMEWORK_IMAGE_WRITEABLEIMAGE_H_ */
