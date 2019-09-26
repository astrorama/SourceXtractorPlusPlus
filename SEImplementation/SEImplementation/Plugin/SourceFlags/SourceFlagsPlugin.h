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
 * Copyright (C) 2012-2020 Euclid Science Ground Segment    
 *  
 * This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General  
 * Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)  
 * any later version.  
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied  
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more  
 * details.  
 *  
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to  
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA  
 */

/**
 * @file SourceFlagsPlugin.h
 *
 * @date Jul 12, 2018
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SOURCEFLAGSPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_SOURCEFLAGSPLUGIN_H_

#include "SourceFlags.h"
#include "SEFramework/Plugin/Plugin.h"
#include "SEImplementation/Plugin/SourceFlags/SourceFlagsTaskFactory.h"

#include <vector>

namespace SExtractor {

class SourceFlagsPlugin : public Plugin {
public:
  virtual ~SourceFlagsPlugin() = default;

  virtual void registerPlugin(PluginAPI &plugin_api) {
    plugin_api.getTaskFactoryRegistry().registerTaskFactory<SourceFlagsTaskFactory, SourceFlags>();
    plugin_api.getOutputRegistry().registerColumnConverter<SourceFlags, std::vector<int64_t>>(
      "source_flags",
      [](const SourceFlags &prop) {
        return flags2long(prop.getSourceFlags());
      },
      "[]",
      "The source flags"
    );
    plugin_api.getOutputRegistry().enableOutput<SourceFlags>("SourceFlags");
  }

  virtual std::string getIdString() const {
    return "source_flags";
  }

private:
}; // end of SourceFlagsPlugin class

}  // namespace SExtractor
#endif /* _SEIMPLEMENTATION_PLUGIN_SOURCEFLAGSPLUGIN_H_ */
