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

#ifndef _SEIMPLEMENTATION_PLUGIN_ONNXATTRIBUTE_H_
#define _SEIMPLEMENTATION_PLUGIN_ONNXATTRIBUTE_H_

#include "SEFramework/Property/Property.h"
#include <memory>
#include <NdArray/NdArray.h>

namespace SourceXtractor {

/**
 * Wraps the tensors outputted by the ONNX runtime
 */
class OnnxProperty : public Property {
public:
  virtual ~OnnxProperty() = default;

  /**
   * Constructor
   * @param output
   *    A map where the key corresponds to the path of one of the ONNX models, and the
   *    value to the corresponding output tensor
   */
  OnnxProperty(std::map<std::string, Euclid::NdArray::NdArray<float>>&& output) : m_output(std::move(output)) {}

  /**
   * @param key
   *    Path to a registered ONNX model
   * @return
   *    Corresponding output tensor
   */
  const Euclid::NdArray::NdArray<float>& getData(const std::string& key) const {
    return m_output.at(key);
  }

private:
  std::map<std::string, Euclid::NdArray::NdArray<float>> m_output;
};

} // end of namespace SourceXtractor

#endif //_SEIMPLEMENTATION_PLUGIN_ONNXATTRIBUTE_H_
