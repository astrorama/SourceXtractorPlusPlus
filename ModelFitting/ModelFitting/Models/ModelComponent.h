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

/**
 * An expended model has one or more components that define its shape. These components
 * must implement this interface.
 */
class ModelComponent {
  
public:

  /***
   * Defines a sample: X coordinate, Y coordinate, and Value at those coordinates
   */
  using ModelSample = std::tuple<double, double, double>;

  /**
   * Destructor
   */
  virtual ~ModelComponent() = default;

  /**
   * Returns the value of the point value of the model at the given coordinates.
   */
  virtual double getValue(double x, double y) = 0;

  /**
   *
   * @param scale
   * @param r_max
   */
  virtual void updateRasterizationInfo(double scale, double r_max) = 0;

  /**
   * Returns the samples computed *only* for the sharp area, if any.
   * @details
   * The returned value is the *integrated* value of the function for the area corresponding to
   * the sample, *not* the value of the function.
   * A pixel may be computed adding up the samples that fall within.
   */
  virtual std::vector<ModelSample> getSharpSampling() = 0;

  /**
   * Returns true if the coordinates fall inside the sharp region
   */
  virtual bool insideSharpRegion(double x, double y) = 0;
  
}; // end of class ModelComponent

} // end of namespace ModelFitting

#endif	/* MODELFITTING_MODELCOMPONENT_H */

