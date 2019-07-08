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
#include <stdint.h>

namespace SExtractor {

/// Flagging of bad sources
enum class Flags : int64_t {
  NONE        = 0,         ///< No flag is set
  BIASED      = 1ll << 0,  ///< The object has bad pixels
  BLENDED     = 1ll << 1,  ///< The object was originally blended with another one
  SATURATED   = 1ll << 2,  ///< At least one pixel of the object is saturated
  BOUNDARY    = 1ll << 3,  ///< The object is truncated (too close to an image boundary)
  NEIGHBORS   = 1ll << 4,  ///< The object has neighbors, bright and close enough
  OUTSIDE     = 1ll << 5,  ///< The object is completely outside of the measurement frame
  PARTIAL_FIT = 1ll << 6,  ///< Some/all of the model parameters could not be fitted
  ERROR       = 1ll << 10, ///< Error flag: something bad happened during the measurement, model fitting, etc.
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

constexpr inline int64_t flags2long(const Flags &a) {
  return static_cast<int64_t>(a);
}

inline std::vector<int64_t> flags2long(const std::vector<Flags> &v) {
  std::vector<int64_t> vl;
  for (auto a : v) {
    vl.emplace_back(flags2long(a));
  }
  return vl;
}

} // end SExtractor

#endif // _SEFRAMEWORK_SOURCE_SOURCEFLAGS_H_
