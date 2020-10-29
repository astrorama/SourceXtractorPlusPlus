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

#ifndef _SEIMPLEMENTATION_PLUGIN_ONNXCONFIG_H_
#define _SEIMPLEMENTATION_PLUGIN_ONNXCONFIG_H_

#include <Configuration/Configuration.h>

namespace SourceXtractor {

/**
 * Handle the configuration of the ONNX plugin
 */
class OnnxConfig: public Euclid::Configuration::Configuration {
public:

  /// Construct a new OnnxConfig
  OnnxConfig(long manager_id);

  /// Destructor
  ~OnnxConfig() override = default;

  /// @copydoc Euclid::Configuration::Configuration::getProgramOptions
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /// @copydoc Euclid::Configuration::Configuration::initialize
  void initialize(const UserValues& args) override;

  /// @return A list of paths to ONNX models
  const std::vector<std::string>& getModels() const {
    return m_onnx_model_paths;
  }

private:
  std::vector<std::string> m_onnx_model_paths;
};

} // end of namespace SourceXtractor

#endif // _SEIMPLEMENTATION_PLUGIN_ONNXCONFIG_H_
