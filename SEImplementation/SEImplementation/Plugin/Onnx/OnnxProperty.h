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
  /**
   * To support dynamic output types (int, float,...) the property stores a pointer to this base type,
   * performing a type erasure on the multidimensional output array
   */
  struct NdWrapperBase {
    virtual ~NdWrapperBase() = default;
  };

  /**
   * Concrete types. We use dynamic_cast to get it back when writing the output.
   */
  template<typename T>
  struct NdWrapper : public NdWrapperBase {
    Euclid::NdArray::NdArray<T> m_ndarray;

    template <typename ...Args>
    NdWrapper(Args&&... args) : m_ndarray(std::forward<Args>(args)...) {}
  };

  /**
   * Destructor
   */
  virtual ~OnnxProperty() = default;

  /**
   * Constructor
   * @param output
   *    A map where the key corresponds to the path of one of the ONNX models, and the
   *    value to the corresponding output tensor
   */
  OnnxProperty(std::map<std::string, std::unique_ptr<NdWrapperBase>>&& output) : m_output(std::move(output)) {}

  /**
   * @param key
   *    Path to a registered ONNX model
   * @return
   *    Corresponding output tensor
   */
  template<typename T>
  const Euclid::NdArray::NdArray<T>& getData(const std::string& key) const {
    return dynamic_cast<NdWrapper<T> *>(m_output.at(key).get())->m_ndarray;
  }

private:
  std::map<std::string, std::unique_ptr<NdWrapperBase>> m_output;
};

} // end of namespace SourceXtractor

#endif //_SEIMPLEMENTATION_PLUGIN_ONNXATTRIBUTE_H_
