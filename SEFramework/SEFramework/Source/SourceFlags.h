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
 * SourceFlags.h
 *
 *  Created on: Oct 19, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEFRAMEWORK_SOURCE_SOURCEFLAGS_H_
#define _SEFRAMEWORK_SOURCE_SOURCEFLAGS_H_

#include <stdint.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <type_traits>

namespace SourceXtractor {

/// Flagging of bad sources
enum class Flags : int64_t {
  NONE              = 0,         ///< No flag is set
  BIASED            = 1ll << 0,  ///< The object has bad pixels
  BLENDED           = 1ll << 1,  ///< The object was originally blended with another one
  SATURATED         = 1ll << 2,  ///< At least one pixel of the object is saturated
  BOUNDARY          = 1ll << 3,  ///< The object is truncated (too close to an image boundary)
  NEIGHBORS         = 1ll << 4,  ///< The object has neighbors, bright and close enough
  OUTSIDE           = 1ll << 5,  ///< The object is completely outside of the measurement frame
  PARTIAL_FIT       = 1ll << 6,  ///< Some/all of the model parameters could not be fitted
  INSUFFICIENT_DATA = 1ll << 7,  ///< There are not enough good pixels to fit the parameters
  ERROR             = 1ll << 10, ///< Error flag: something bad happened during the measurement, model fitting, etc.
  SENTINEL          = 1ll << 11, ///< Used to find the boundary of possible values
};

/// String representation of the flags
const std::map<Flags, std::string> FlagsStr = {
  {Flags::NONE, "NONE"},
  {Flags::BIASED, "BIASED"},
  {Flags::BLENDED, "BLENDED"},
  {Flags::BOUNDARY, "BOUNDARY"},
  {Flags::NEIGHBORS, "NEIGHBORS"},
  {Flags::OUTSIDE, "OUTSIDE"},
  {Flags::PARTIAL_FIT, "PARTIAL_FIT"},
  {Flags::INSUFFICIENT_DATA, "INSUFFICIENT_DATA"},
  {Flags::ERROR, "ERROR"}
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

inline std::ostream& operator<<(std::ostream& out, Flags flags) {
  std::underlying_type<Flags>::type i;
  bool some_printed = false;
  for (i = static_cast<decltype(i)>(Flags::BIASED);
       i < static_cast<decltype(i)>(Flags::SENTINEL); i <<= 1) {
    if ((flags & static_cast<Flags>(i)) != Flags::NONE) {
      if (some_printed)
        out << " | ";
      else
        out << "(";
      out << FlagsStr.at(static_cast<Flags>(i));
      some_printed = true;
    }
  }
  if (some_printed)
    out << ")";
  else
    out << "NONE";
  return out;
}

} // end SourceXtractor

#endif // _SEFRAMEWORK_SOURCE_SOURCEFLAGS_H_
