/*
 * ProgressPrinterFactory.h
 *
 *  Created on: Mar 19, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSPRINTERFACTORY_H
#define _SEMAIN_PROGRESSPRINTERFACTORY_H

#include <boost/date_time/posix_time/posix_time_config.hpp>
#include "SEFramework/Configuration/Configurable.h"
#include "SEMain/ProgressPrinter.h"

namespace SExtractor {

class ProgressPrinterFactory : public Configurable {
public:
  virtual ~ProgressPrinterFactory() = default;

  ProgressPrinterFactory();

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

  void configure(Euclid::Configuration::ConfigManager& manager) override;

  std::shared_ptr<ProgressPrinter> createPrinter(void) const;

private:
  boost::posix_time::time_duration m_min_interval;
};

}

#endif // _SEMAIN_PROGRESSPRINTERFACTORY_H
