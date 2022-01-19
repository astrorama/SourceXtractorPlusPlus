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
 * MoffatModelFittingUtils.h
 *
 *  Created on: 2019 M02 20
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELEVALUATOR_H_
#define _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELEVALUATOR_H_

#include "ModelFitting/Models/ExtendedModel.h"
#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEImplementation/Image/ImageInterfaceTraits.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFitting.h"

namespace SourceXtractor {

class MoffatModelEvaluator: public Property {
public:
  explicit MoffatModelEvaluator(const MoffatModelFitting& model);

  double getValue(double x, double y) const {
    return m_model->getValue(x, y);
  }

  unsigned getIterations() const {
    return m_iterations;
  }

private:
  std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>> m_model;
  unsigned m_iterations;
};

//ModelFitting::ExtendedModel createMoffatModel();

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELEVALUATOR_H_ */
