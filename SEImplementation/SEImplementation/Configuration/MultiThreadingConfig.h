/*
 * MultiThreadingConfig.h
 *
 *  Created on: May 22, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_MULTITHREADINGCONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_MULTITHREADINGCONFIG_H_

#include "Configuration/Configuration.h"

namespace SExtractor {

class MultiThreadingConfig : public Euclid::Configuration::Configuration {
public:
  MultiThreadingConfig(long manager_id);

  virtual ~MultiThreadingConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  unsigned int getThreadsNb() const {
    return m_threads_nb;
  }

private:
  unsigned int m_threads_nb;
};


}


#endif /* _SEIMPLEMENTATION_CONFIGURATION_MULTITHREADINGCONFIG_H_ */
