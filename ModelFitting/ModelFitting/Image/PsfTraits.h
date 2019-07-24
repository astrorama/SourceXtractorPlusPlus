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
