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
 * DetectionFrameGroupStampPlugin.cpp
 *
 *  Created on: May 5, 2017
 *      Author: mschefer
 */

#include "SEFramework/Plugin/StaticPlugin.h"

#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h"
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStampTaskFactory.h"
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStampPlugin.h"
#include "SEImplementation/Image/ImageInterfaceTraits.h"

namespace SExtractor {

static StaticPlugin<DetectionFrameGroupStampPlugin> detection_frame_group_stamp_plugin;

void DetectionFrameGroupStampPlugin::registerPlugin(PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<DetectionFrameGroupStampTaskFactory, DetectionFrameGroupStamp>();

  plugin_api.getOutputRegistry().registerColumnConverter<DetectionFrameGroupStamp, int>(
    "group_stamp_top",
    [](const DetectionFrameGroupStamp &d) {
      return d.getTopLeft().m_y;
    },
    "[pixel]",
    "Maximum y-coordinate of the detection group"
  );
  plugin_api.getOutputRegistry().registerColumnConverter<DetectionFrameGroupStamp, int>(
    "group_stamp_left",
    [](const DetectionFrameGroupStamp &d) {
      return d.getTopLeft().m_x;
    },
    "[pixel]",
    "Minimum x-coordinate of the detection group"
  );
  plugin_api.getOutputRegistry().registerColumnConverter<DetectionFrameGroupStamp, int>(
    "group_stamp_width",
    [](const DetectionFrameGroupStamp &d) {
      return d.getStamp().getWidth();
    },
    "[pixel]",
    "Width of the detection group"
  );
  plugin_api.getOutputRegistry().registerColumnConverter<DetectionFrameGroupStamp, int>(
    "group_stamp_height",
    [](const DetectionFrameGroupStamp &d) {
      return d.getStamp().getHeight();
    },
    "[pixel]",
    "Height of the detection group"
  );
  plugin_api.getOutputRegistry().enableOutput<DetectionFrameGroupStamp>("GroupStamp");
}

std::string DetectionFrameGroupStampPlugin::getIdString() const {
  return "DetectionFrameGroupStamp";
}

}


