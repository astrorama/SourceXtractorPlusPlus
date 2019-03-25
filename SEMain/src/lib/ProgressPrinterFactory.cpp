#include "SEMain/ProgressPrinterFactory.h"
#include "SEMain/ProgressPrinterConfiguration.h"
#include "SEMain/ProgressLogger.h"
#include "SEMain/ProgressBar.h"

namespace SExtractor {

ProgressPrinterFactory::ProgressPrinterFactory(): m_disable_progress_bar{false} {}

void ProgressPrinterFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<ProgressPrinterConfiguration>();
}

void ProgressPrinterFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto progress_config = manager.getConfiguration<ProgressPrinterConfiguration>();
  m_min_interval = progress_config.getMinPrintInterval();
  m_disable_progress_bar = progress_config.isProgressBarDisabled();
}

std::shared_ptr<ProgressPrinter> ProgressPrinterFactory::createProgressPrinter() const {
  std::vector<std::string> entries{"Detected", "Deblended", "Measured", "Segmentation"};

  if (::isatty(::fileno(stderr)) && !m_disable_progress_bar) {
    auto progress_bar = ProgressBar::getInstance();
    progress_bar->setElements(entries);
    return progress_bar;
  }
  return std::make_shared<ProgressLogger>(m_min_interval);
}

} // end SExtractor
