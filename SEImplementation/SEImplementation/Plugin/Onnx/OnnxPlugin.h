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

#ifndef _SEIMPLEMENTATION_PLUGIN_ONNXPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_ONNXPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"
#include <ElementsKernel/Logging.h>

namespace SourceXtractor {

/**
 * The ONNX plugin wraps a machine learning model stored using
 * the "Open Neural Network Exchange" format. SourceXtractor will
 * feed the model a stamp with the size the model requires as an input.
 * For the output column, the 'domain' field + the graph name from the ONNX model will
 * be used: i.e. ch.unige.astro_galaxy_class
 * @see
 *  https://onnx.ai/
 */
class OnnxPlugin : public Plugin {
public:

  /// Destructor
  virtual ~OnnxPlugin() = default;

  /// @copydoc Plugin::getIdString
  std::string getIdString() const override;

  /// @copydoc Plugin::registerPlugin
  void registerPlugin(PluginAPI& plugin_api) override;
};

/// Logger for the ONNX plugin
extern Elements::Logging onnx_logger;

} // end of namespace SourceXtractor

#endif // _SEIMPLEMENTATION_PLUGIN_ONNXPLUGIN_H_
