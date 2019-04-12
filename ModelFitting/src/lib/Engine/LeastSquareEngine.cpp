/**
 * @file LeastSquareEngine.cpp
 * @date April, 2019
 * @author Alejandro Alvarez Ayllon
 */

#include <ElementsKernel/Exception.h>
#include <boost/algorithm/string.hpp>
#include "ModelFitting/Engine/LeastSquareEngine.h"

namespace ModelFitting {

// We use this idiom to make sure the map is initialized on the first need
// Otherwise, this may be initialized *after* the concrete implementations try to register themselves
static std::map<std::string, LeastSquareEngine::FactoryMethod>& getEngineFactories() {
  static std::map<std::string, LeastSquareEngine::FactoryMethod> engine_factories;
  return engine_factories;
}


void LeastSquareEngine::registerEngine(const std::string& name, FactoryMethod factory_method) {
  if (getEngineFactories().find(name) != getEngineFactories().end()) {
    throw Elements::Exception() << "A LeastSquareEngine named " << name << " has already been registered";
  }
  getEngineFactories()[boost::algorithm::to_lower_copy(name)] = factory_method;
}

std::vector<std::string> LeastSquareEngine::getImplementations() {
  std::vector<std::string> keys;
  for (auto &e : getEngineFactories()) {
    keys.emplace_back(e.first);
  }
  return keys;
}

std::shared_ptr<LeastSquareEngine> LeastSquareEngine::create(const std::string& name, unsigned max_iterations) {
  auto factory = getEngineFactories().find(boost::algorithm::to_lower_copy(name));
  if (factory == getEngineFactories().end()) {
    throw Elements::Exception() << "No LeastSquareEngine named " << name << " has been registered";
  }
  return factory->second(max_iterations);
}

} // end namespace ModelFitting
