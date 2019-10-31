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
 * BlendedFlagPlugin.h
 *
 *  Created on: Oct 29, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAGPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAGPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"
#include "BlendedFlagTaskFactory.h"

namespace SourceXtractor {

class BlendedFlagPlugin : public Plugin {
public:
  virtual ~BlendedFlagPlugin() = default;

  virtual void registerPlugin(PluginAPI &plugin_api) {
    plugin_api.getTaskFactoryRegistry().registerTaskFactory<BlendedFlagTaskFactory, BlendedFlag>();
  }

  virtual std::string getIdString() const {
    return "blended_flag";
  }
};

} // end SourceXtractor

#endif // _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAGPLUGIN_H_
