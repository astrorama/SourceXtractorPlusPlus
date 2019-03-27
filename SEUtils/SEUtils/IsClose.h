/**
 * @file IsClose.h
 * @date 27/09/18
 * @author Alejandro Alvarez Ayllon
 */

#ifndef SEUTILS_ISCLOSE_H
#define SEUTILS_ISCLOSE_H

namespace SExtractor {

bool isClose(double a, double b, double atol = 1e-8, double rtol = 1e-5) {
  return std::abs(a - b) <= (atol + rtol * std::abs(b));
}

} // end SExtractor

#endif // SEUTILS_ISCLOSE_H
