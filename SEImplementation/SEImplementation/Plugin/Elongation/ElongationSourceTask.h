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
 * @file ElongationSourceTask.h
 *
 * @date Jan. 17, 2020
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_ELONGATIONSOURCETASK_H_
#define _SEIMPLEMENTATION_PLUGIN_ELONGATIONSOURCETASK_H_

#include "SEFramework/Task/SourceTask.h"
#include "SEImplementation/Plugin/Elongation/Elongation.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"

namespace SourceXtractor {
class ElongationSourceTask : public SourceTask {
public:
  virtual ~ElongationSourceTask() = default;

  virtual void computeProperties(SourceInterface& source) const {
    // get the relevant properties
    const auto& a_image = source.getProperty<ShapeParameters>().getEllipseA();
    const auto& b_image = source.getProperty<ShapeParameters>().getEllipseB();

    // compute and set the property
    source.setProperty<Elongation>((SeFloat) a_image / b_image);
  };
private:
}; // End of ElongationSourceTask class
} // namespace SourceXtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_ELONGATIONSOURCETASK_H_ */
