/*
 * ProgressReporterConfiguration.h
 *
 *  Created on: Mar 19, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSPRINTERCONFIGURATION_H
#define _SEMAIN_PROGRESSPRINTERCONFIGURATION_H


#include <Configuration/Configuration.h>
#include <chrono>

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

  std::chrono::steady_clock::duration getMinPrintInterval() const {
    return m_min_interval;
  }

  bool isProgressBarDisabled() const {
    return m_disable_progress_bar;
  }

private:
  std::chrono::steady_clock::duration m_min_interval;
  bool m_disable_progress_bar;
};

} // end SExtractor

#endif // _SEMAIN_PROGRESSPRINTERCONFIGURATION_H
