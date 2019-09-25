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
