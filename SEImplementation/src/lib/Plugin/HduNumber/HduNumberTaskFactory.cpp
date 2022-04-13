/** Copyright © 2019-2022 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include "SEImplementation/Plugin/HduNumber/HduNumber.h"
#include "SEImplementation/Plugin/HduNumber/HduNumberTask.h"
#include "SEImplementation/Plugin/HduNumber/HduNumberTaskFactory.h"

namespace SourceXtractor {

void HduNumberTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager&) const {
}

void HduNumberTaskFactory::configure(Euclid::Configuration::ConfigManager&) {
}

std::shared_ptr<Task> HduNumberTaskFactory::createTask(const PropertyId& property_id) const {
  if (property_id == PropertyId::create<HduNumber>()) {
    return std::make_shared<HduNumberTask>();
  } else {
    return nullptr;
  }
}

}

