/** 
 * @file PositionedModel.h
 * @date August 26, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_POSITIONEDMODEL_H
#define	MODELFITTING_POSITIONEDMODEL_H

#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Parameters/ReferenceUpdater.h"

namespace ModelFitting {

class PositionedModel {
  
public:
  
  PositionedModel(BasicParameter& x, BasicParameter& y);
          
  PositionedModel(PositionedModel&& other);

  virtual ~PositionedModel() = default;
  
  double getX() const;
  
  double getY() const;
  
private:
  
  double m_x;
  double m_y;
  ReferenceUpdater m_x_updater;
  ReferenceUpdater m_y_updater;
  
}; // end of class ConstantModel

} // end of namespace ModelFitting

#endif	/* MODELFITTING_POSITIONEDMODEL_H */

