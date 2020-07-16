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
 * @file VignetPlugin.h
 *
 * @date Jan 17, 2020
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_VIGNETPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_VIGNETPLUGIN_H_

#include <NdArray/NdArray.h>
#include "SEFramework/Plugin/Plugin.h"
#include "SEImplementation/Plugin/Vignet/VignetTaskFactory.h"
#include "Vignet.h"
#include "VignetArray.h"

namespace SourceXtractor {

template<typename T>
using NdArray = Euclid::NdArray::NdArray<T>;

class VignetPlugin : public Plugin {
public:
  virtual ~VignetPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) {
    plugin_api.getTaskFactoryRegistry().registerTaskFactory<VignetTaskFactory, Vignet, VignetArray>();
    plugin_api.getOutputRegistry().registerColumnConverter<VignetArray, NdArray<DetectionImage::PixelType >>(
      "vignet",
      [](const VignetArray& prop) {
        return prop.getVignets();
      },
      "count",
      "The object vignet data"
    );
    plugin_api.getOutputRegistry().enableOutput<VignetArray>("Vignet");
  }

  virtual std::string getIdString() const {
    return "Vignet";
  }

private:
}; // end of VignetPlugin class

}  // namespace SourceXtractor
#endif /* _SEIMPLEMENTATION_PLUGIN_VIGNETPLUGIN_H_ */
