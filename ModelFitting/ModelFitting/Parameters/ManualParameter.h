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
