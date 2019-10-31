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
 * @file CoreThresholdPartitionPlugin
 *
 * @date May 27, 2019
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_CORETHRESHOLDPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_CORETHRESHOLDPLUGIN_H_

#include "CoreThresholdPartitionTaskFactory.h"
#include "NCorePixel.h"
#include "SEFramework/Plugin/Plugin.h"

namespace SourceXtractor {
class CoreThresholdPartitionPlugin : public Plugin {
public:
  virtual ~CoreThresholdPartitionPlugin() = default;
  virtual void registerPlugin(PluginAPI& plugin_api) {
    plugin_api.getTaskFactoryRegistry().registerTaskFactory<CoreThresholdPartitionTaskFactory, NCorePixel>();
    plugin_api.getOutputRegistry().registerColumnConverter<NCorePixel, int64_t>(
            "n_core_pixel",
            [](const NCorePixel& prop){
              return prop.getNCorePixel();
            },
            "[]",
            "Number of core pixels used for partitioning"
    );
    plugin_api.getOutputRegistry().enableOutput<NCorePixel>("NCorePixel");
  }
  virtual std::string getIdString() const {
    return "n_core_pixel";
  }
private:
}; // end of CoreThresholdPartitionPlugin class
}  // namespace SExtractor
#endif /* _SEIMPLEMENTATION_PLUGIN_SNRLEVELPLUGIN_H_ */
