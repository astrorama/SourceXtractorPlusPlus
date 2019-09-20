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
 * @file DataVsModelInputTraits.h
 * @date August 20, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_DATAVSMODELINPUTTRAITS_H
#define	MODELFITTING_DATAVSMODELINPUTTRAITS_H

namespace ModelFitting {

/**
 * @class DataVsModelInputTraits
 * 
 * @brief
 * Traits class the DataVsModelResiduals class uses for accessing its inputs
 * 
 * @details
 * To achieve an extra level of abstraction, the DataVsModelResiduals is not
 * accessing directly its templated input parameters. Instead, it uses the
 * static methods of this class, with the correct InputType template parameter.
 * This way, classes which are not providing the required methods can still be
 * used as DataVsModelResiduals inputs.
 * 
 * The default implementation of the DataVsModelInputTraits is designed to
 * cover STL containers. To use a custom class which is not compliant with
 * this interface as input, a specialization of the DataVsModelInputTraits can
 * be created.
 * 
 * @tparam InputType
 *    The type of the parameter the traits manages
 */
template <typename InputType>
struct DataVsModelInputTraits {
  
  /// The type of the iterator used for accessing the input elements. It must
  /// provide the input iterator operations.
  using iterator = typename InputType::const_iterator;
  
  /// Returns an iterator to the first element of the input
  static iterator begin(InputType& input) {
    return input.begin();
  }
  
  /// Returns an iterator to one after the last element of the input
  static iterator end(InputType& input) {
    return input.end();
  }
  
  /// Returns the size of the input
  static size_t size(const InputType& input) {
    return input.size();
  }
  
};

} // end of namespace ModelFitting

#endif	/* MODELFITTING_DATAVSMODELINPUTTRAITS_H */

