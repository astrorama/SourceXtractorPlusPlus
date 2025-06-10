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

#include "SEImplementation/Grouping/AssocCriteria.h"

#include "SEImplementation/Plugin/AssocMode/AssocMode.h"

namespace SourceXtractor {

bool AssocCriteria::shouldGroup(const SourceInterface& first, const SourceInterface& second) const {
  auto first_id = first.getProperty<AssocMode>().getGroupId();
  auto second_id = second.getProperty<AssocMode>().getGroupId();
  return first_id != 0 && first_id == second_id;
}

} // SourceXtractor namespace

