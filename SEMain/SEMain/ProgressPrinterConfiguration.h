/*
 * ProgressPrinterConfiguration.h
 *
 *  Created on: Mar 19, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSPRINTERCONFIGURATION_H
#define _SEMAIN_PROGRESSPRINTERCONFIGURATION_H


#include <Configuration/Configuration.h>
#include <boost/date_time/posix_time/posix_time_config.hpp>

namespace SExtractor {

class ProgressPrinterConfiguration: public Euclid::Configuration::Configuration {
public:
  ProgressPrinterConfiguration(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  void preInitialize(const UserValues& args) override;
  void initialize(const UserValues& args) override;

  boost::posix_time::time_duration getMinPrintInterval() const {
    return m_min_interval;
  }

private:
  boost::posix_time::time_duration m_min_interval;
};

} // end SExtractor

#endif // _SEMAIN_PROGRESSPRINTERCONFIGURATION_H
