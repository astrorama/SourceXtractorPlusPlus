#include <unistd.h>
#include "SEMain/ProgressPrinterFactory.h"
#include "SEMain/ProgressPrinterConfiguration.h"
#include "SEMain/ProgressLogger.h"
#include "SEMain/ProgressBar.h"

namespace SExtractor {

ProgressPrinterFactory::ProgressPrinterFactory() {}

void ProgressPrinterFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<ProgressPrinterConfiguration>();
}

void ProgressPrinterFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto progress_config = manager.getConfiguration<ProgressPrinterConfiguration>();
  m_min_interval = progress_config.getMinPrintInterval();
}

std::shared_ptr<ProgressPrinter> ProgressPrinterFactory::createPrinter() const {
  std::initializer_list<std::string> entries{"Detected", "Deblended", "Measured", "Segmentation"};
  if (::isatty(::fileno(stderr))) {
    return std::make_shared<ProgressBar>(std::cerr, entries);
  }
  return std::make_shared<ProgressLogger>(m_min_interval, entries);
}

} // end SExtractor
