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
 * FitsWcsSerializable.h
 *
 *  Created on: Sep 23, 2026
 *      Author: embray
 */

#ifndef _SEFRAMEWORK_FITS_FITSWCSSERIALIZABLE_H_
#define _SEFRAMEWORK_FITS_FITSWCSSERIALIZABLE_H_

#include <map>
#include <string>

namespace SourceXtractor {

/**
 * Mixin for coordinate systems that can be written out as FITS header cards
 *
 * Not every CoordinateSystem can be: a GWCS read from an ASDF file generally has
 * no FITS equivalent.  Implement this alongside CoordinateSystem where such a
 * representation does exist; FitsImageSource writes the cards for coordinate
 * systems that provide it, and omits them otherwise.
 *
 * TODO: There *are* at least one or two schemes for storing YAML-serialized
 * GWCS in FITS. I forget if it is a series of header cards, or stored in its
 * own HDU data.  In the latter case this interface would need to be expanded
 * to support this form of serialization.
 */
class FitsWcsSerializable {
public:
  virtual ~FitsWcsSerializable() = default;

  virtual std::map<std::string, std::string> getFitsHeaders() const = 0;
};

}  // namespace SourceXtractor

#endif /* _SEFRAMEWORK_FITS_FITSWCSSERIALIZABLE_H_ */
