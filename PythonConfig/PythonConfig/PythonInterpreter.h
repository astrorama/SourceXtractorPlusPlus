/* 
 * @file PythonInterpreter.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _PYTHONCONFIG_PYTHONINTERPRETER_H
#define _PYTHONCONFIG_PYTHONINTERPRETER_H

#include <string>
#include <map>
#include <PythonConfig/PyMeasurementImage.h>

namespace SExtractor {

class PythonInterpreter {
  
public:
  
  static PythonInterpreter& getSingleton();
  
  void runCode(const std::string& code);
  
  void runFile(const std::string& filename);
  
  virtual ~PythonInterpreter();
  
  std::map<int, PyMeasurementImage> getMeasurementImages();
  
private:
  
  PythonInterpreter();
  
};

} // end of namespace SExtractor

#endif // _PYTHONCONFIG_PYTHONINTERPRETER_H

