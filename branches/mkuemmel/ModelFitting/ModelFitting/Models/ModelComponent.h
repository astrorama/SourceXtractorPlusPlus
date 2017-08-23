/** 
 * @file ModelComponent.h
 * @date August 24, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_MODELCOMPONENT_H
#define	MODELFITTING_MODELCOMPONENT_H

#include <vector>
#include <tuple>
#include <memory>
#include <utility>

namespace ModelFitting {

class ModelComponent {
  
public:
  
  using ModelSample = std::tuple<double, double, double>;
  
  virtual ~ModelComponent() = default;
  
  virtual double getValue(double x, double y) = 0;
  
  virtual void updateRasterizationInfo(double scale, double r_max) = 0;
  
  virtual std::vector<ModelSample> getSharpSampling() = 0;
  
  virtual bool insideSharpRegion(double x, double y) = 0;
  
}; // end of class ModelComponent

} // end of namespace ModelFitting

#endif	/* MODELFITTING_MODELCOMPONENT_H */

