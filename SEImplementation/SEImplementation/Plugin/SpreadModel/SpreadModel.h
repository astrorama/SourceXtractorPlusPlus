/** Copyright © 2019-2026 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
#ifndef _SEIMPLEMENTATION_PLUGIN_SPREADMODEL_SPREADMODEL_H_
#define _SEIMPLEMENTATION_PLUGIN_SPREADMODEL_SPREADMODEL_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SourceXtractor {

/**
 * @brief Spread-model value and uncertainty for one measurement frame.
 * @details The property index is the measurement frame ID.
 * Both quantities are dimensionless. Rejected frames store NaN for both fields.
 * The uncertainty propagates pixel noise with the fitted templates held fixed.
 */
class SpreadModel : public Property {
public:

  SpreadModel(SeFloat spread_model, SeFloat spread_model_error)
    : m_spread_model(spread_model), m_spread_model_error(spread_model_error) {}

  virtual ~SpreadModel() = default;

  SeFloat getSpreadModel() const {
    return m_spread_model;
  }

  SeFloat getSpreadModelError() const {
    return m_spread_model_error;
  }

private:
  SeFloat m_spread_model;
  SeFloat m_spread_model_error;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_SPREADMODEL_SPREADMODEL_H_ */
