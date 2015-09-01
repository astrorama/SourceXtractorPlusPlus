/** 
 * @file PointModel.h
 * @date August 26, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_POINTMODEL_H
#define	MODELFITTING_POINTMODEL_H

#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Models/PositionedModel.h"
#include "ModelFitting/Models/ConstantModel.h"

namespace ModelFitting {

class PointModel : public PositionedModel, public ConstantModel {
  
public:
  
  PointModel(BasicParameter& x, BasicParameter& y, BasicParameter& value);
  
  PointModel(PointModel&& other); 
  
  virtual ~PointModel();
  
};

} // end of namespace ModelFitting

#endif	/* MODELFITTING_POINTMODEL_H */

