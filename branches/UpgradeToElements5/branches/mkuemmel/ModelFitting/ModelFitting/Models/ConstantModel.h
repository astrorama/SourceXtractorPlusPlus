/** 
 * @file ConstantModel.h
 * @date August 26, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_CONSTANTMODEL_H
#define	MODELFITTING_CONSTANTMODEL_H

#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Parameters/ReferenceUpdater.h"

namespace ModelFitting {

class ConstantModel {
  
public:
  
  ConstantModel(BasicParameter& value);
          
  ConstantModel(ConstantModel&& other);

  virtual ~ConstantModel();
  
  double getValue() const;
  
private:
  
  double m_value;
  ReferenceUpdater m_value_updater;
  
}; // end of class ConstantModel

} // end of namespace ModelFitting

#endif	/* MODELFITTING_CONSTANTMODEL_H */

