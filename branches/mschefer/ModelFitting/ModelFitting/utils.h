/** 
 * @file utils.h
 * @date September 2, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef SEXTRACTOR_UTILS_H
#define	SEXTRACTOR_UTILS_H

#include <memory>
#include <utility>

/// Creates a unique_ptr to an object of type T, which is constructed by using
/// the given arguments
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>{new T(std::forward<Args>(args)...)};
}

#endif	/* SEXTRACTOR_UTILS_H */

