/* 
 * @file PythonConfig.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_PYTHONCONFIG_H
#define _SEIMPLEMENTATION_CONFIGURATION_PYTHONCONFIG_H

#include <Configuration/Configuration.h>
#include <SEImplementation/PythonConfig/PythonInterpreter.h>

namespace SExtractor {

class PythonConfig : public Euclid::Configuration::Configuration {
  
public:
  
  PythonConfig(long manager_id);
  
  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;

  void preInitialize(const UserValues& args) override;

  void initialize(const UserValues& args) override;
  
  PythonInterpreter& getInterpreter() const;

};

}

#endif // _SEIMPLEMENTATION_CONFIGURATION_PYTHONCONFIG_H

