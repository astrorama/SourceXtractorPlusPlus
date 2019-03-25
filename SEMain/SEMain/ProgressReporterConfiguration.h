/*
 * ProgressReporterConfiguration.h
 *
 *  Created on: Mar 19, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSPRINTERCONFIGURATION_H
#define _SEMAIN_PROGRESSPRINTERCONFIGURATION_H


#include <Configuration/Configuration.h>
#include <boost/date_time/posix_time/posix_time_config.hpp>

namespace SExtractor {

/**
 * @class ProgressReporterConfiguration
 * Configuration for the progress reporting.
 */
class ProgressReporterConfiguration: public Euclid::Configuration::Configuration {
public:
  ProgressReporterConfiguration(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  void preInitialize(const UserValues& args) override;
  void initialize(const UserValues& args) override;

  boost::posix_time::time_duration getMinPrintInterval() const {
    return m_min_interval;
  }

  bool isProgressBarDisabled() const {
    return m_disable_progress_bar;
  }

  bool isLogFileSet() const {
    return m_log_file_set;
  }

private:
  boost::posix_time::time_duration m_min_interval;
  bool m_disable_progress_bar;
  bool m_log_file_set;
};

} // end SExtractor

#endif // _SEMAIN_PROGRESSPRINTERCONFIGURATION_H
