/*
 * RngConfig.h
 *
 *  Created on: Mar 21, 2024
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_RNGCONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_RNGCONFIG_H_

#include "Configuration/Configuration.h"
#include "AlexandriaKernel/ThreadPool.h"

namespace SourceXtractor {

class RngConfig : public Euclid::Configuration::Configuration {
public:
  explicit RngConfig(long manager_id);

  virtual ~RngConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  unsigned int getSeed() const {
    return m_seed;
  }

private:
  unsigned int m_seed;
};


}


#endif /* _SEIMPLEMENTATION_CONFIGURATION_RNGCONFIG_H_ */
