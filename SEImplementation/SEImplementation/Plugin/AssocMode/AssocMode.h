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

#ifndef _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODE_H_
#define _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODE_H_

#include "NdArray/NdArray.h"

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"
#include "SEFramework/Source/SourceFlags.h"

namespace SourceXtractor {

/**
 * @class AssocMode
 * @brief
 */
class AssocMode : public Property {
public:
  template<typename T>
  using NdArray = Euclid::NdArray::NdArray<T>;

  /**
   * @brief Destructor
   */
  virtual ~AssocMode() = default;

  AssocMode() : m_has_assoc(false), m_assoc_data(std::vector<size_t>({0})), m_ref_frame_pixel_radius(1) {
  }

  AssocMode(bool has_assoc, const std::vector<double>& assoc_data, double ref_frame_pixel_radius=0.0) :
      m_has_assoc(has_assoc), m_assoc_data(std::vector<size_t>({assoc_data.size()})),
      m_ref_frame_pixel_radius(ref_frame_pixel_radius) {
    for (size_t i=0; i<assoc_data.size(); i++) {
      m_assoc_data.at(i) = assoc_data[i];
    }
  }

  bool getMatch() const {
    return m_has_assoc;
  }

  const NdArray<SeFloat>& getAssocValues() const {
    return m_assoc_data;
  }

  double getRefFramePixelRadius() const {
    return m_ref_frame_pixel_radius;
  }

private:
  bool m_has_assoc;

  NdArray<SeFloat> m_assoc_data;
  double m_ref_frame_pixel_radius;
};

} /* namespace SourceXtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODE_H_ */
