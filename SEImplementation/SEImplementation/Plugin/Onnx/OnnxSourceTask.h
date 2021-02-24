/** Copyright © 2020 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#ifndef _SEIMPLEMENTATION_PLUGIN_ONNXSOURCETASK_H_
#define _SEIMPLEMENTATION_PLUGIN_ONNXSOURCETASK_H_

#include "SEFramework/Task/SourceTask.h"
#include "SEImplementation/Common/OnnxModel.h"

namespace SourceXtractor {

/**
 * Run a set of ONNX models over a source
 */
class OnnxSourceTask: public SourceTask {
public:

  /**
   * Constructor
   * @param models
   *    Reference to the loaded ONNX models
   */
  OnnxSourceTask(const std::vector<OnnxModel>& models);

  /**
   * Destructor
   */
  ~OnnxSourceTask() override = default;

  ///@copydoc SourceTask::computeProperties
  void computeProperties(SourceInterface& source) const override;

private:
  const std::vector<OnnxModel>& m_models;
};

} // end of namespace SourceXtractor

#endif // _SEIMPLEMENTATION_PLUGIN_ONNXSOURCETASK_H_
