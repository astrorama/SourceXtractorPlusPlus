/** Copyright © 2021 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#ifndef _SEUTILS_MMAPALLOCATOR_H
#define _SEUTILS_MMAPALLOCATOR_H

#include <new>
#include <sys/mman.h>

namespace SourceXtractor {

/**
 * Allocate memory via mmap
 * @warning Do not use if the allocations are expected to be small
 * @tparam T
 */
template <typename T>
struct MmapAllocator {
  using value_type = T;

  MmapAllocator() = default;

  template <typename U>
  constexpr MmapAllocator(const MmapAllocator<U>&) noexcept {}

  T* allocate(std::size_t n) {
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
#if __cplusplus < 201103L || (!defined(__llvm__) && !defined(__INTEL_COMPILER) && defined(__GNUC__) && __GNUC__ == 4 && __GNUC_MINOR__ <= 8)
      throw std::bad_alloc();
#else
      throw std::bad_array_new_length();
#endif
    }
    T* ptr = static_cast<T*>(mmap(nullptr, n * sizeof(T), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (ptr == MAP_FAILED) {
      throw std::bad_alloc();
    }
    return ptr;
  }

  void deallocate(T* ptr, std::size_t n) {
    munmap(ptr, n * sizeof(T));
  }
};

template <typename T, typename U>
bool operator==(const MmapAllocator<T>&, const MmapAllocator<U>&) {
  return sizeof(T) == sizeof(U);
}

template <typename T, typename U>
bool operator!=(const MmapAllocator<T>&, const MmapAllocator<U>&) {
  return sizeof(T) != sizeof(U);
}

}  // namespace SourceXtractor

#endif  // _SEUTILS_MMAPALLOCATOR_H
