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

#include "SEImplementation/Plugin/Vignet/Vignet.h"
#include "SEImplementation/Plugin/Vignet/VignetArray.h"
#include "SEImplementation/Plugin/Vignet/VignetArraySourceTask.h"

namespace SourceXtractor {

VignetArraySourceTask::VignetArraySourceTask(const std::vector<unsigned> images): m_images{images} {
}

void VignetArraySourceTask::computeProperties(SourceInterface& source) const {
  std::vector<std::reference_wrapper<const Vignet>> vignets;
  for (auto img_idx : m_images) {
    vignets.emplace_back(source.getProperty<Vignet>(img_idx));
  }
  source.setProperty<VignetArray>(vignets);
}

} // end of namespace SourceXtractor
