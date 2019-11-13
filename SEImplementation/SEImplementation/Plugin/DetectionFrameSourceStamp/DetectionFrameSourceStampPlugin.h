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
 * DetectionFrameSourceStampPlugin.h
 *
 *  Created on: Aug 5, 2016
 *      Author: mschefer
 */

#ifndef SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMESOURCESTAMPPLUGIN_H_
#define SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMESOURCESTAMPPLUGIN_H_

#include "SEFramework/Plugin/Plugin.h"

namespace SExtractor {

class DetectionFrameSourceStampPlugin : public Plugin {

public:

  /**
   * @brief Destructor
   */
  virtual ~DetectionFrameSourceStampPlugin() = default;

  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;

private:

}; /* End of DetectionFrameSourceStampPlugin class */

}


#endif /* SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_DETECTIONFRAMESOURCESTAMPPLUGIN_H_ */
