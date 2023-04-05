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
 * @file VignetSourceTask.h
 *
 * @date Jan. 23, 2020
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_VIGNETSOURCETASK_H_
#define _SEIMPLEMENTATION_PLUGIN_VIGNETSOURCETASK_H_

#include "SEFramework/Task/SourceTask.h"
#include <array>

namespace SourceXtractor {
class VignetSourceTask : public SourceTask {
public:
  VignetSourceTask(unsigned instance, std::array<int, 2> vignet_size, double vignet_default_pixval) :
    m_instance(instance),
    m_vignet_size(vignet_size),
    m_vignet_default_pixval((SeFloat) vignet_default_pixval) {};

  virtual ~VignetSourceTask() = default;

  virtual void computeProperties(SourceInterface& source) const;

private:
  unsigned m_instance;
  std::array<int, 2> m_vignet_size;
  SeFloat m_vignet_default_pixval;
}; // End of VignetSourceTask class

} // namespace SourceXtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_VIGNETSOURCETASK_H_ */
