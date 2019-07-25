/*
 * MoffatModelFittingUtils.h
 *
 *  Created on: 2019 M02 20
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTINGUTILS_H_
#define _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTINGUTILS_H_

#include "ModelFitting/Models/ExtendedModel.h"
#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Image/ImageInterfaceTraits.h"

namespace SExtractor {

class MoffatModelEvaluator {
public:
  MoffatModelEvaluator(const SourceInterface& source);
  double getValue(double x, double y) const {
    return m_model->getValue(x, y);
  }

private:
  std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>> m_model;
};

//ModelFitting::ExtendedModel createMoffatModel();

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTINGUTILS_H_ */
