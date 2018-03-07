/*
 * ImageTile.hm
 *
 *  Created on: Feb 20, 2018
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_IMAGE_IMAGETILE_H_
#define _SEFRAMEWORK_IMAGE_IMAGETILE_H_

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"

namespace SExtractor {

class ImageTileBase {
public:
  virtual ~ImageTileBase() = default;

  virtual int getTileSize() const = 0;

private:
};

template <typename T>
class ImageTile : public ImageTileBase {
public:
  ImageTile(int x, int y, int width, int height) : m_x(x), m_y(y), m_max_x(x+width), m_max_y(y+height) {
    m_tile_image = VectorImage<T>::create(width, height);
  }

  bool isPixelInTile(int x, int y) const {
    return x >= m_x && y >= m_y && x < m_max_x && y < m_max_y;
  }

  T getValue(int x, int y) const {
    assert(isPixelInTile(x,y));

    return m_tile_image->getValue(x-m_x, y-m_y);
  }

  std::shared_ptr<VectorImage<T>> getImage() {
    return m_tile_image;
  }

  int getTileSize() const override {
    return (m_max_x-m_x) * (m_max_y-m_y) * sizeof(T);
  }


private:
  int m_x, m_y;
  int m_max_x, m_max_y;
  std::shared_ptr<VectorImage<T>> m_tile_image;
};


}


#endif /* _SEFRAMEWORK_IMAGE_IMAGETILE_H_ */
