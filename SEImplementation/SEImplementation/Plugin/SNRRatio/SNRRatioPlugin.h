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
 * @file SNRRatioPlugin.h
 *
 * @date Apr 29, 2020
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SNRRATIOPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_SNRRATIOPLUGIN_H_

#include "SNRRatio.h"
#include "SEFramework/Plugin/Plugin.h"
#include "SEImplementation/Plugin/SNRRatio/SNRRatioTaskFactory.h"

namespace SourceXtractor {
class SNRRatioPlugin : public Plugin {
public:
  virtual ~SNRRatioPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) {
    plugin_api.getTaskFactoryRegistry().registerTaskFactory<SNRRatioTaskFactory, SNRRatio>();
    plugin_api.getOutputRegistry().registerColumnConverter<SNRRatio, float>(
      "snrratio",
      [](const SNRRatio& prop) {
        return prop.getSNRRatio();
      },
      "",
      "The object signal-to-noise ratio"
    );
    plugin_api.getOutputRegistry().enableOutput<SNRRatio>("SNRRatio");
  }

  virtual std::string getIdString() const {
    return "snrratio";
  }

private:
}; // end of SNRRatioPlugin class
}  // namespace SourceXtractor
#endif /* _SEIMPLEMENTATION_PLUGIN_SNRRATIOPLUGIN_H_ */
