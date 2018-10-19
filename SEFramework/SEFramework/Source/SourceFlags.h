/*
 * SourceFlags.h
 *
 *  Created on: Oct 19, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEFRAMEWORK_SOURCE_SOURCEFLAGS_H_
#define _SEFRAMEWORK_SOURCE_SOURCEFLAGS_H_

#include <type_traits>
#include <vector>

namespace SExtractor {

/// Flagging of bad sources
/// @note Backwards compatible with sextractor flags!
enum class Flags : unsigned long {
  NONE      = 0,      ///< No flag is set
  BIASED    = 1 << 0, ///< The object has neighbors, bright and close enough, or bad pixels
  BLENDED   = 1 << 1, ///< The object was originally blended with another one.
  SATURATED = 1 << 2, ///< At least one pixel of the object is saturated.
  BOUNDARY  = 1 << 3, /// <The object is truncates (to close to an image boundary).
};


constexpr inline Flags operator|(const Flags &a, const Flags &b) {
  typedef typename std::underlying_type<Flags>::type base_int_t;
  return static_cast<Flags>(static_cast<base_int_t>(a) | static_cast<base_int_t>(b));
}

constexpr inline Flags operator&(const Flags &a, const Flags &b) {
  typedef typename std::underlying_type<Flags>::type base_int_t;
  return static_cast<Flags>(static_cast<base_int_t>(a) & static_cast<base_int_t>(b));
}

constexpr Flags operator*(const Flags &a, const bool b) {
  return b ? a : Flags::NONE;
}

inline Flags &operator|=(Flags &a, const Flags &b) {
  a = a | b;
  return a;
}

constexpr inline unsigned long flags2long(const Flags &a) {
  return static_cast<unsigned long>(a);
}

inline std::vector<long> flags2long(const std::vector<Flags> &v) {
  std::vector<long> vl;
  for (auto a : v) {
    vl.emplace_back(flags2long(a));
  }
  return vl;
}

} // end SExtractor

#endif // _SEFRAMEWORK_SOURCE_SOURCEFLAGS_H_
