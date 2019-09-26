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
 * AutoPhotometryFlagTask.h
 *
 *  Created on: Oct 10, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYFLAGTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYFLAGTASK_H_

#include "SEUtils/Types.h"
#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class AutoPhotometryFlagTask: public SourceTask {
public:

  virtual ~AutoPhotometryFlagTask() = default;

  AutoPhotometryFlagTask(SeFloat kron_factor, SeFloat kron_minrad)
  : m_kron_factor{kron_factor}, m_kron_minrad{kron_minrad} {}

  virtual void computeProperties(SourceInterface& source) const override;

private:
  SeFloat m_kron_factor;
  SeFloat m_kron_minrad;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYFLAGTASK_H_
