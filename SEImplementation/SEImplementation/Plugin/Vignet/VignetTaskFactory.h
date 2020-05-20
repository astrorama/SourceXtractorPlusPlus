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
 * @file VignetTaskFactory.h
 *
 * @date Jan 17, 2020
 * @author mkuemmel@usm.lmu.de
 */
#ifndef _SEIMPLEMENTATION_PLUGIN_VIGNETTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_VIGNETTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"
#include "SEImplementation/Plugin/Vignet/Vignet.h"
#include "SEImplementation/Plugin/Vignet/VignetConfig.h"
#include "SEImplementation/Plugin/Vignet/VignetSourceTask.h"

namespace SourceXtractor {
class VignetTaskFactory : public TaskFactory {
public:
  VignetTaskFactory() {}

  virtual ~VignetTaskFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
    manager.registerConfiguration<VignetConfig>();
  };

  void configure(Euclid::Configuration::ConfigManager& manager) {

    // get the input parameter
    auto vignet_config = manager.getConfiguration<VignetConfig>();
    m_vignet_size = vignet_config.getVignetSize();
    m_vignet_default_pixval = vignet_config.getVignetDefaultPixval();
  };

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const {
    if (property_id == PropertyId::create<Vignet>()) {
      return std::make_shared<VignetSourceTask>(m_vignet_size, m_vignet_default_pixval);
    }
    else {
      return nullptr;
    }
  }

private:
  std::array<int, 2> m_vignet_size;
  double m_vignet_default_pixval;
}; // end of VignetTaskFactory class

}  // namespace SourceXtractor
#endif /* _SEIMPLEMENTATION_PLUGIN_VIGNETTASKFACTORY_H_ */
