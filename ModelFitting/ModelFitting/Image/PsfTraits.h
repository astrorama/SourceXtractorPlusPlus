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
 * @file PsfTraits.h
 * @date July 18, 2019
 * @author Alejandro Álvarez Ayllón
 */

#ifndef MODELFITTING_PSFTRAITS_H
#define MODELFITTING_PSFTRAITS_H

namespace ModelFitting {

/**
 * The default PsfTrait remains compatible with PSF implementations
 * that do not have the concept of a context or a prepare method.
 * This way they do not have to be modified.
 *
 * PSF types that have this concept should specialize the trait with has_context = true
 * and the appropiate context_t
 */
template <typename PsfType>
struct PsfTraits {
  using context_t = std::false_type;
  static constexpr bool has_context = false;
};

} // end namespace ModelFitting

#endif // MODELFITTING_PSFTRAITS_H
