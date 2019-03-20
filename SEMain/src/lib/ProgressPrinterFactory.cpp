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
  std::vector<std::string> entries{"Detected", "Deblended", "Measured", "Segmentation"};

  if (::isatty(::fileno(stderr))) {
    auto progress_bar = ProgressBar::getInstance();
    progress_bar->setElements(entries);
    return progress_bar;
  }
  return std::make_shared<ProgressLogger>(m_min_interval);
}

} // end SExtractor
