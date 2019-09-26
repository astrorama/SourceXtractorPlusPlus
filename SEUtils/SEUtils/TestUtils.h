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
/**
 * @file CompareImages.h
 * @date 11/09/18
 * @author Alejandro Alvarez Ayllon
 */


#ifndef _COMPAREIMAGES_H
#define _COMPAREIMAGES_H

#include "SEUtils/IsClose.h"

namespace SExtractor {

template <typename T, typename U>
boost::test_tools::predicate_result compareImages(
  const T& ref, const U& val, double atol = 1e-8, double rtol = 1e-5) {
  boost::test_tools::predicate_result res(true);

  for (int x = 0; x < ref->getWidth(); ++x) {
    for (int y = 0; y < ref->getHeight(); ++y) {
      auto expected = ref->getValue(x, y);
      auto value = val->getValue(x, y);
      if (!isClose(expected, value, atol, rtol)) {
        res = false;
        res.message() << "Not matching values at position " << x << "," << y
                      << ": " << expected << " != " << value << "\n";
      }
    }
  }

  return res;
}

template <typename T, typename U>
boost::test_tools::predicate_result compareCollections(
  const T& ref, const U& val, double atol = 1e-8, double rtol = 1e-5) {
  boost::test_tools::predicate_result res(true);

  auto ref_i = std::begin(ref);
  auto val_i = std::begin(val);
  int i = 0;

  while (ref_i != std::end(ref) && val_i != std::end(val)) {

    if (!isClose(*ref_i, *val_i, atol, rtol)) {
      res = false;
      res.message() << "Not matching values at position " << i << ": " << *ref_i << " != " << *val_i << "\n";
    }

    ++ref_i;
    ++val_i;
    ++i;
  }

  if (ref_i != std::end(ref) || val_i != std::end(val)) {
    res = false;
    res.message() << "The sequences have different length!" << "\n";
  }

  return res;
}

boost::test_tools::predicate_result checkIsClose(double ref, const double val, double atol = 1e-8, double rtol = 1e-5) {
  boost::test_tools::predicate_result res(true);
  if (!isClose(ref, val, atol, rtol)) {
    res = false;
    res.message() << "Values not close enough: " << ref << " " << val << " with absolute tolerance " << atol
                  << " and relative " << rtol << "\n";
  }
  return res;
}

} // end SExtractor

#endif // _COMPAREIMAGES_H
