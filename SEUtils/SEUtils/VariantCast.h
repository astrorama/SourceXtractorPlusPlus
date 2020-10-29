/** Copyright © 2020 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#ifndef _SEUTILS_VARIANTCAST_H_
#define _SEUTILS_VARIANTCAST_H_

#include <boost/lexical_cast.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>

namespace SourceXtractor {

template<typename To>
class VariantCastVisitor : public boost::static_visitor<To> {
public:
  template<typename From>
  To operator()(const From& from) const {
    return boost::lexical_cast<To>(from);
  }
};

template<typename To, typename From>
To VariantCast(const From& from) {
  return boost::apply_visitor(VariantCastVisitor<To>(), from);
}

} // end of namespace SourceXtractor

#endif /* _SEUTILS_VARIANTCAST_H_ */
