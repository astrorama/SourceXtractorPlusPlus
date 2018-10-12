/*
 * FlexibleModelFittingParameterManager.h
 *
 *  Created on: Oct 9, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGPARAMETERMANAGER_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGPARAMETERMANAGER_H_

#include <map>

#include "ModelFitting/Engine/EngineParameterManager.h"
#include "SEFramework/Source/SourceInterface.h"

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h"

namespace SExtractor {

// Needed to store the source in a reference_wrapper
inline bool operator<(std::reference_wrapper<const SourceInterface> a, std::reference_wrapper<const SourceInterface> b) {
  return &a.get() < &b.get();
}

class FlexibleModelFittingParameterManager {

public:

  FlexibleModelFittingParameterManager() {}
  virtual ~FlexibleModelFittingParameterManager() {}


  std::shared_ptr<ModelFitting::BasicParameter> getParameter(
      const SourceInterface& source, std::shared_ptr<FlexibleModelFittingParameter> parameter) const {
    return m_params.at(std::make_tuple(std::cref(source), parameter));
  }

  void add(const SourceInterface& source, std::shared_ptr<FlexibleModelFittingParameter> parameter) {
    m_params[std::make_tuple(std::cref(source), parameter)] = parameter->create(m_engine_manager);
  }

  ModelFitting::EngineParameterManager& getEngineParameterManager() {
    return m_engine_manager;
  }

private:
  std::map<std::tuple<std::reference_wrapper<const SourceInterface>, std::shared_ptr<FlexibleModelFittingParameter>>, std::shared_ptr<ModelFitting::BasicParameter>> m_params;
  ModelFitting::EngineParameterManager m_engine_manager {};

};

}



#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGPARAMETERMANAGER_H_ */
