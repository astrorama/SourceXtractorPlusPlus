/*
 * LockedWriteableImage.h
 *
 *  Created on: Jul 7, 2020
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_IMAGE_LOCKEDWRITEABLEIMAGE_H_
#define _SEIMPLEMENTATION_IMAGE_LOCKEDWRITEABLEIMAGE_H_

#include "SEFramework/Image/WriteableImage.h"

#include "SEImplementation/Measurement/MultithreadedMeasurement.h"

namespace SourceXtractor {

template <typename T>
class LockedWriteableImage: public WriteableImage<T> {
protected:
  LockedWriteableImage(std::shared_ptr<WriteableImage<T>> img) : m_img{img}, m_lock(MultithreadedMeasurement::g_global_mutex) {
  }

public:
  template<typename... Args>
  static std::shared_ptr<LockedWriteableImage<T>> create(Args &&... args) {
    return std::shared_ptr<LockedWriteableImage<T>>(new LockedWriteableImage{std::forward<Args>(args)...});
  }

  std::string getRepr() const override {
    return "LockedWriteableImage(" + m_img->getRepr() + ")";
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

  void setValue(int x, int y, T value) override {
    m_img->setValue(x, y, value);
  }

  std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height) const override {
    return m_img->getChunk(x, y, width, height);
  }

private:
  std::shared_ptr<WriteableImage<T>> m_img;
  std::lock_guard<std::recursive_mutex> m_lock;
};

}

#endif /* _SEIMPLEMENTATION_IMAGE_LOCKEDWRITEABLEIMAGE_H_ */
