/*
 * TransformModelComponent.h
 *
 *  Created on: Jun 20, 2018
 *      Author: mschefer
 */

#ifndef _MODELFITTING_MODELS_TRANSFORMMODELCOMPONENT_H_
#define _MODELFITTING_MODELS_TRANSFORMMODELCOMPONENT_H_

#include <memory> // for std::unique_ptr
#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Parameters/ReferenceUpdater.h"
#include "ModelFitting/Models/ModelComponent.h"

namespace ModelFitting {

/**
 * Decorates a model component, transforming the *axes* by the given transformation matrix
 *
 * @note
 *  Unlike ScaledModelComponent, this transformation keeps the integrated value
 */
class TransformModelComponent : public ModelComponent {

public:

  /**
   * Constructor
   * @param component
   *    The component to be decorated. The ownership is acquired by the new object.
   * @param transform
   *    The transformation matrix (row major order)
   */
  TransformModelComponent(std::unique_ptr<ModelComponent> component, std::tuple<double, double, double, double> transform);

  TransformModelComponent(TransformModelComponent&& other);

  virtual ~TransformModelComponent();

  double getValue(double x, double y) override;

  void updateRasterizationInfo(double scale, double r_max) override;

  std::vector<ModelSample> getSharpSampling() override;

  bool insideSharpRegion(double x, double y) override;

private:

  std::unique_ptr<ModelComponent> m_component;

  double m_transform[4];
  double m_inv_transform[4];

}; // end of class TransformModelComponent

} // end of namespace ModelFitting


#endif /* _MODELFITTING_MODELS_TRANSFORMMODELCOMPONENT_H_ */
