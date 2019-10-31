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
 * JacobianPlugin.cpp
 *
 *  Created on: Oct 08, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include "SEFramework/Plugin/StaticPlugin.h"
#include "SEImplementation/Plugin/Jacobian/Jacobian.h"
#include "SEImplementation/Plugin/Jacobian/JacobianTaskFactory.h"
#include "SEImplementation/Plugin/Jacobian/JacobianPlugin.h"

namespace SourceXtractor {

static StaticPlugin<JacobianPlugin> jacobian_plugin;

std::string JacobianPlugin::getIdString() const {
  return "JacobianPlugin";
}

void JacobianPlugin::registerPlugin(PluginAPI &plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<JacobianTaskFactory, JacobianSource, JacobianGroup>();
}

} // end SourceXtractor
