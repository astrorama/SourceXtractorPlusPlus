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
 * MoffatModelFittingTask.h
 *
 *  Created on: May 2, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTINGTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTINGTASK_H_

#include "SEFramework/Task/SourceTask.h"

#include "SEImplementation/Image/ImagePsf.h"

namespace SourceXtractor {

class MoffatModelFittingTask : public SourceTask {

public:
  MoffatModelFittingTask(const std::string& least_squares_engine, unsigned int max_iterations)
    : m_least_squares_engine(least_squares_engine), m_max_iterations(max_iterations) {}

  virtual ~MoffatModelFittingTask() = default;

  virtual void computeProperties(SourceInterface& source) const override;

private:

  std::string m_least_squares_engine;
  unsigned int m_max_iterations;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTINGTASK_H_ */
