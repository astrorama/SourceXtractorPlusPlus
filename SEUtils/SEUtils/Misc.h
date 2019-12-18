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

#ifndef _SEUTILS_MISC_H_
#define _SEUTILS_MISC_H_

#include <type_traits>

namespace SourceXtractor {

template <typename T>
T nextPowerOfTwo(T v) {
  static_assert(std::is_unsigned<T>::value, "Type T must be unsigned");
  v--;
  for (size_t i = 1; i < sizeof(v) * 8U; i *= 2) {
    v |= v >> i;
  }
  v++;
  return v;
}

}

#endif /* _SEUTILS_MISC_H_ */
