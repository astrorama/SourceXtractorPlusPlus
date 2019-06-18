/*
 * ProgressReporterFactory.h
 *
 *  Created on: Mar 19, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSPRINTERFACTORY_H
#define _SEMAIN_PROGRESSPRINTERFACTORY_H

#include <chrono>
#include "SEFramework/Configuration/Configurable.h"
#include "SEMain/ProgressMediator.h"

namespace SExtractor {

/**
 * @class ProgressReporterFactory
 * Abstracts away the creation of concrete ProgressReporters
 */
class ProgressReporterFactory : public Configurable {
public:
  virtual ~ProgressReporterFactory() = default;

  ProgressReporterFactory();

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

  void configure(Euclid::Configuration::ConfigManager& manager) override;

  std::shared_ptr<ProgressMediator> createProgressMediator(void) const;

private:
  std::chrono::steady_clock::duration m_min_interval;
  bool m_disable_progress_bar;
  bool m_log_file;
};

}

#endif // _SEMAIN_PROGRESSPRINTERFACTORY_H
