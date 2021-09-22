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
 * @file PythonInterpreter.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _SEIMPLEMENTATION_PYTHONINTERPRETER_H
#define _SEIMPLEMENTATION_PYTHONINTERPRETER_H

#include <string>
#include <map>
#include <vector>
#include <SEFramework/CoordinateSystem/CoordinateSystem.h>
#include <SEImplementation/PythonConfig/PyMeasurementImage.h>
#include <SEImplementation/PythonConfig/PyAperture.h>
#include <SEImplementation/PythonConfig/PyOutputWrapper.h>

namespace SourceXtractor {

class PythonInterpreter {
  
public:
  
  static PythonInterpreter& getSingleton();
  
  void runCode(const std::string& code);
  
  void runFile(const std::string& filename, const std::vector<std::string>& argv);

  virtual ~PythonInterpreter() = default;
  
  std::map<int, PyMeasurementImage> getMeasurementImages();

  std::map<int, PyAperture> getApertures();

  std::vector<std::pair<std::string, std::vector<int>>> getModelFittingOutputColumns();

  std::map<std::string, std::vector<int>> getApertureOutputColumns();

  std::map<int, boost::python::object> getConstantParameters();
  
  std::map<int, boost::python::object> getFreeParameters();
  
  std::map<int, boost::python::object> getDependentParameters();
  
  std::map<int, boost::python::object> getPriors();
  
  std::map<int, boost::python::object> getConstantModels();

  std::map<int, boost::python::object> getPointSourceModels();
  
  std::map<int, boost::python::object> getSersicModels();
  
  std::map<int, boost::python::object> getExponentialModels();
  
  std::map<int, boost::python::object> getDeVaucouleursModels();
  
  std::map<int, boost::python::object> getOnnxModels();

  std::map<int, std::vector<int>> getFrameModelsMap();

  std::map<std::string, boost::python::object> getModelFittingParams();

  std::vector<boost::python::object> getMeasurementGroups();

  void setCoordinateSystem(std::shared_ptr<CoordinateSystem> coordinate_system);

private:
  
  PythonInterpreter();
  PyOutputWrapper m_out_wrapper, m_err_wrapper;
};

} // end of namespace SourceXtractor

#endif // _SEIMPLEMENTATION_PYTHONINTERPRETER_H

