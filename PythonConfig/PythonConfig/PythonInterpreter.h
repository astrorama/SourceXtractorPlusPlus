/* 
 * @file PythonInterpreter.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _PYTHONCONFIG_PYTHONINTERPRETER_H
#define _PYTHONCONFIG_PYTHONINTERPRETER_H

#include <string>
#include <map>
#include <PythonConfig/MeasurementImage.h>

namespace SExtractor {

class PythonInterpreter {
  
public:
  
  static PythonInterpreter& getSingleton();
  
  void runCode(const std::string& code);
  
  virtual ~PythonInterpreter();
  
  std::map<int, MeasurementImage> getMeasurementImages();
  
private:
  
  PythonInterpreter();
  
};

} // end of namespace SExtractor

#endif // _PYTHONCONFIG_PYTHONINTERPRETER_H

