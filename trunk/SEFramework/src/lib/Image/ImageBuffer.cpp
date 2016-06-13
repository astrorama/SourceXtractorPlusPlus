/**
 * @file src/lib/Image/ImageBuffer.cpp
 * @date 06/03/16
 * @author mschefer
 */

#include "SEFramework/Image/ImageBuffer.h"

namespace SExtractor {

ImageBuffer::ImageBuffer(int width, int height) : m_width(width), m_height(height) {
  m_data.resize(m_width * m_height);
}

double ImageBuffer::getValue(int x, int y) const {
  return m_data[x + y * m_width];
}

int ImageBuffer::getWidth() const {
  return m_width;
}

int ImageBuffer::getHeight() const {
  return m_height;
}

} // SEFramework namespace

