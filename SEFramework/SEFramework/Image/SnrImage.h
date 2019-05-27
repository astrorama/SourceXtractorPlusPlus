/*
 * SnrImage.h
 *
 *  Created on: May 24, 2019
 *      Author: mkuemmel
 */

#ifndef _SEFRAMEWORK_IMAGE_SNRIMAGE_H_
#define _SEFRAMEWORK_IMAGE_SNRIMAGE_H_

#include <memory>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageBase.h"
#include "SEFramework/Image/ConstantImage.h"

namespace SExtractor {

/**
 * @class SnrImage
 * @brief Used to subtract a constant value from an Image
 *
 */
template <typename T>
class SnrImage : public ImageBase<T> {

protected:

  SnrImage(std::shared_ptr<const Image<T>> image, std::shared_ptr<const Image<T>> variance_map) :
    m_image(image), m_variance_map(variance_map) {
    assert(m_image->getWidth() == m_variance_map->getWidth());
    assert(m_image->getHeight() == m_variance_map->getHeight());
  };

public:

  /**
   * @brief Destructor
   */
  virtual ~SnrImage() = default;

  static std::shared_ptr<SnrImage<T>> create(
      std::shared_ptr<const Image<T>> image, std::shared_ptr<const Image<T>> variance_map) {
    return std::shared_ptr<SnrImage<T>>(new SnrImage<T>(image, variance_map));
  }

  using Image<T>::getValue;
  T getValue(int x, int y) const override {
    return m_image->getValue(x, y) / sqrt(m_variance_map->getValue(x, y));
  }

  int getWidth() const override {
    return m_image->getWidth();
  }

  int getHeight() const override {
    return m_image->getHeight();
  }

private:
  std::shared_ptr<const Image<T>> m_image, m_variance_map;

}; /* End of SnrImage class */

} /* namespace SExtractor */

#endif /* _SEFRAMEWORK_IMAGE_SNRIMAGE_H_ */
