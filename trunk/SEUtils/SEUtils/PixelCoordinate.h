/**
 * @file SEUtils/PixelCoordinate.h
 * @date 06/07/16
 * @author mschefer
 */

#ifndef _SEUTILS_PIXELCOORDINATE_H
#define _SEUTILS_PIXELCOORDINATE_H

#include <functional>

namespace SEUtils {

/**
 * @class PixelCoordinate
 * @brief
 *
 */

struct PixelCoordinate {
  int m_x, m_y;

  PixelCoordinate(int x, int y) : m_x(x), m_y(y) {}

  bool operator==(const PixelCoordinate& other) const {
    return m_x == other.m_x && m_y == other.m_y;
  }

  PixelCoordinate operator+(const PixelCoordinate& other) const {
    return PixelCoordinate(m_x + other.m_x, m_y + other.m_y);
  }

  PixelCoordinate operator-(const PixelCoordinate& other) const {
    return PixelCoordinate(m_x - other.m_x, m_y - other.m_y);
  }
};


} /* namespace SEUtils */


namespace std {

template <>
struct hash<SEUtils::PixelCoordinate>
{
  std::size_t operator()(const SEUtils::PixelCoordinate& coord) const {
     return hash<int>()(coord.m_x) ^ hash<int>()(coord.m_y);
  }
};

} // namespace std


#endif
