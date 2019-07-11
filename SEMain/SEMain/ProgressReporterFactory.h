/*
 * ProgressReporterFactory.h
 *
 *  Created on: Mar 19, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSPRINTERFACTORY_H
#define _SEMAIN_PROGRESSPRINTERFACTORY_H

#include <chrono>
#include "SEMain/ProgressMediator.h"

namespace SExtractor {

/**
 * @class ProgressReporterFactory
 *  Abstracts away the creation of concrete ProgressReporters
 * @note
 *  This class is not a Configurable because it needs to be started before the rest of the
 *  configurables. Otherwise, their logging can not be intercepted.
 */
class ProgressReporterFactory {
public:
  virtual ~ProgressReporterFactory() = default;

  ProgressReporterFactory();

  void addOptions(boost::program_options::options_description &options) const;

  void configure(const std::map<std::string, boost::program_options::variable_value> &args);

  std::shared_ptr<ProgressMediator> createProgressMediator(void) const;

private:
  std::chrono::steady_clock::duration m_min_interval;
  bool m_disable_progress_bar;
};

}

#endif // _SEMAIN_PROGRESSPRINTERFACTORY_H
