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
