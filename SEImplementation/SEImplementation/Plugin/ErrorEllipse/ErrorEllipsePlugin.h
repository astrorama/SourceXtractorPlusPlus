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
 * ErrorEllipsePlugin.h
 *
 *  Created on: Feb 11, 20122
 *      Author: mkuemmel
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_ERRORELLIPSE_ERRORELLIPSEPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_ERRORELLIPSE_ERRORELLIPSEPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SourceXtractor {

class ErrorEllipsePlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~ErrorEllipsePlugin() = default;

  void registerPlugin(PluginAPI& plugin_api) override;
  std::string getIdString() const override;

private:

};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_ERRORELLIPSE_ERRORELLIPSEPLUGIN_H_ */
