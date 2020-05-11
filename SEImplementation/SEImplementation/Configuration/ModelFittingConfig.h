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
 * @file ModelFittingConfig.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_MODELFITTINGCONFIG_H
#define _SEIMPLEMENTATION_CONFIGURATION_MODELFITTINGCONFIG_H

#include <vector>

#include <boost/python/extract.hpp>
#include <boost/python/object.hpp>
#include <boost/python/tuple.hpp>

#include <Configuration/Configuration.h>

#include <SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.h>
#include <SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingModel.h>
#include <SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingFrame.h>
#include <SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingPrior.h>

#include "SEUtils/Python.h"


namespace SourceXtractor {

namespace py = boost::python;

/**
 * @brief Hold a reference to a Python object
 * @details
 *  A boost::python::object contains a pointer to the underlying Python struct, which is
 *  copied as-is (shared) when copied. When the boost::python::object is destroyed, it checks,
 *  and then decrements, the reference count. This destruction is *not* thread safe, as the pointer
 *  is not protected by a mutex or anything.
 *  This class holds a single reference to the Python object, and relies on the mechanism of
 *  std::shared_ptr to destroy the object once there is no one using it. std::shared_ptr *is*
 *  thread safe, unlike boost::python::object.
 */
class PyObjectHolder {
  public:
    PyObjectHolder(py::object&& obj): m_obj_ptr(std::make_shared<py::object>(obj)) {}

    PyObjectHolder(const PyObjectHolder&) = default;
    PyObjectHolder(PyObjectHolder&&) = default;

    operator const py::object&() const {
      return *m_obj_ptr;
    }

    const py::object& operator *() const {
      return *m_obj_ptr;
    }

    py::object attr(const char *name) {
      return m_obj_ptr->attr(name);
    }


  private:
    std::shared_ptr<py::object> m_obj_ptr;
};


class DependentFunctionFactory {
public:
  virtual FlexibleModelFittingDependentParameter::ValueFunc create(PyObjectHolder params) const = 0;
};

class ModelFittingConfig : public Euclid::Configuration::Configuration {
  
public:
  
  ModelFittingConfig(long manager_id);

  ~ModelFittingConfig();
  
  void initialize(const UserValues& args) override;
  
  const std::map<int, std::shared_ptr<FlexibleModelFittingParameter>>& getParameters() const;
  
  const std::map<int, std::shared_ptr<FlexibleModelFittingModel>>& getModels() const;
  const std::vector<std::shared_ptr<FlexibleModelFittingFrame>>& getFrames() const;
  
  const std::map<int, std::shared_ptr<FlexibleModelFittingPrior>>& getPriors() const;
  
  const std::vector<std::pair<std::string, std::vector<int>>>& getOutputs() const;

  std::string getLeastSquaresEngine() const {
    return m_least_squares_engine;
  }

  unsigned int getMaxIterations() const { return m_max_iterations; }
  double getModifiedChiSquaredScale() const { return m_modified_chi_squared_scale; }

private:
  std::string m_least_squares_engine;
  unsigned int m_max_iterations {0};
  double m_modified_chi_squared_scale {10.};
  
  std::map<int, std::shared_ptr<FlexibleModelFittingParameter>> m_parameters;
  std::map<int, std::shared_ptr<FlexibleModelFittingModel>> m_models;
  std::vector<std::shared_ptr<FlexibleModelFittingFrame>> m_frames;
  std::map<int, std::shared_ptr<FlexibleModelFittingPrior>> m_priors;
  std::vector<std::pair<std::string, std::vector<int>>> m_outputs;

  std::map<std::string, std::shared_ptr<DependentFunctionFactory>> m_predefined_functions;

  /// The initialization code calls Python. It gets wrapped here so we can catch any
  /// potential error and translate into a Elements exception
  void initializeInner();
};

}

#endif // _SEIMPLEMENTATION_CONFIGURATION_MODELFITTINGCONFIG_H

