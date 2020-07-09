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
 * @file LeastSquareEngineManager.cpp
 * @date April 16, 2019
 * @author Alejandro Alvarez Ayllon
 */

#include <ElementsKernel/Exception.h>
#include <boost/algorithm/string.hpp>
#include "ModelFitting/Engine/LeastSquareEngineManager.h"

namespace ModelFitting {

// We use this idiom to make sure the map is initialized on the first need
// Otherwise, this may be initialized *after* the concrete implementations try to register themselves
static std::map<std::string, LeastSquareEngineManager::FactoryMethod>& getEngineFactories() {
  static std::map<std::string, LeastSquareEngineManager::FactoryMethod> engine_factories;
  return engine_factories;
}


void LeastSquareEngineManager::registerEngine(const std::string& name, FactoryMethod factory_method) {
  if (getEngineFactories().find(name) != getEngineFactories().end()) {
    throw Elements::Exception() << "A LeastSquareEngine named " << name << " has already been registered";
  }
  getEngineFactories()[boost::algorithm::to_lower_copy(name)] = factory_method;
}

std::vector<std::string> LeastSquareEngineManager::getImplementations() {
  std::vector<std::string> keys;
  for (auto &e : getEngineFactories()) {
    keys.emplace_back(e.first);
  }
  return keys;
}

std::string LeastSquareEngineManager::getDefault() {
  auto known_engines = ModelFitting::LeastSquareEngineManager::getImplementations();
  std::string default_engine;

  if (std::find(known_engines.begin(), known_engines.end(), "gsl") != known_engines.end()) {
    return "gsl";
  }
  else if (!known_engines.empty()) {
    return known_engines.front();
  }
  return "";
}

std::shared_ptr<LeastSquareEngine> LeastSquareEngineManager::create(const std::string& name, unsigned max_iterations) {
  auto factory = getEngineFactories().find(boost::algorithm::to_lower_copy(name));
  if (factory == getEngineFactories().end()) {
    std::ostringstream known_str;
    if (!getEngineFactories().empty()) {
      known_str << ". Known engines: ";
      for (auto &e : getEngineFactories()) {
        known_str << e.first << " ";
      }
    }
    throw Elements::Exception() << "No LeastSquareEngine named " << name << " has been registered" << known_str.str();
  }
  return factory->second(max_iterations);
}

} // end namespace ModelFitting
