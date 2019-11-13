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
 * AutoPhotometry.h
 *
 *  Created on: Nov 23, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYARRAYTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYARRAYTASK_H_

#include "SEUtils/Types.h"
#include "SEFramework/Task/SourceTask.h"

namespace SourceXtractor {

/**
 * Merge all the AutoPhotometry measurements into a single vector so it can
 * be written as a vector column
 */
class AutoPhotometryArrayTask: public SourceTask {
public:

  virtual ~AutoPhotometryArrayTask() = default;

  AutoPhotometryArrayTask(const std::vector<unsigned> &images): m_images(images) {}

  virtual void computeProperties(SourceInterface& source) const override;

private:
  std::vector<unsigned> m_images;
};

} // end SourceXtractor

#endif // _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYARRAYTASK_H_
