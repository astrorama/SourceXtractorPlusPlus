/**
 * @file ManualParameter.h
 * @date August 11, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_MANUALPARAMETER_H
#define MODELFITTING_MANUALPARAMETER_H

#include "ModelFitting/Parameters/BasicParameter.h"

namespace ModelFitting {

class ManualParameter : public BasicParameter {

public:

  ManualParameter(const double value) : BasicParameter{value} { }

  virtual ~ManualParameter() = default;

  void setValue(const double new_value) {
    BasicParameter::setValue(new_value);
  }

};

}

#endif  /* MODELFITTING_MANUALPARAMETER_H */
