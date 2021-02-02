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

#ifndef _SEIMPLEMENTATION_IMAGE_LOCKEDIMAGE_H_
#define _SEIMPLEMENTATION_IMAGE_LOCKEDIMAGE_H_

#include "SEFramework/Image/Image.h"
#include "SEImplementation/Measurement/MultithreadedMeasurement.h"

namespace SourceXtractor {

template <typename T>
class LockedImage: public Image<T> {
protected:
  LockedImage(std::shared_ptr<const Image<T>> img) : m_img{img}, m_lock(MultithreadedMeasurement::g_global_mutex) {
  }

public:
  template<typename... Args>
  static std::shared_ptr<LockedImage<T>> create(Args &&... args) {
    return std::shared_ptr<LockedImage<T>>(new LockedImage{std::forward<Args>(args)...});
  }

  std::string getRepr() const override {
    return "LockedImage(" + m_img->getRepr() + ")";
  }

  int getWidth() const override {
    return m_img->getWidth();
  }

  int getHeight() const override {
    return m_img->getHeight();
  }

  T getValue(int x, int y) const override {
    return m_img->getValue(x, y);
  }

  std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const override{
    return m_img->getChunk(x, y, width, height);
  }

private:
  std::shared_ptr<const Image<T>> m_img;
  std::lock_guard<std::recursive_mutex> m_lock;
};


}

#endif /* _SEIMPLEMENTATION_IMAGE_LOCKEDIMAGE_H_ */
