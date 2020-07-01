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

#ifndef _SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEIMAGES_DETECTIONFRAMEIMAGES_H_
#define _SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEIMAGES_DETECTIONFRAMEIMAGES_H_

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageBase.h"
#include "SEFramework/Image/ImageChunk.h"

#include "SEFramework/Property/Property.h"
#include "SEFramework/Frame/Frame.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

#include "SEImplementation/Measurement/MultithreadedMeasurement.h"


namespace SourceXtractor {

template <typename T>
class LockedImage: public ImageBase<T> {
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

private:
  std::shared_ptr<const Image<T>> m_img;
  std::lock_guard<std::recursive_mutex> m_lock;
};

class DetectionFrameImages : public Property {

public:
  virtual ~DetectionFrameImages() = default;

  DetectionFrameImages( std::shared_ptr<DetectionImageFrame> frame, int width, int height)
    : m_width(width), m_height(height), m_frame(frame) {}

  std::shared_ptr<Image<SeFloat>> getLockedImage(FrameImageLayer layer) const {
    std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);
    return LockedImage<SeFloat>::create(m_frame->getImage(layer));
  }

  std::shared_ptr<ImageChunk<DetectionImage::PixelType>> getImageChunk(FrameImageLayer layer, int x, int y, int width, int height) const {
    std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

    return m_frame->getImage(layer)->getChunk(x, y, width, height);
  }

  int getWidth() const {
    return m_width;
  }

  int getHeight() const {
    return m_height;
  }

private:
  int m_width;
  int m_height;
  std::shared_ptr<DetectionImageFrame> m_frame;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMEIMAGES_DETECTIONFRAMEIMAGES_H_ */
