#include "SEMain/ProgressReporterFactory.h"
#include "SEMain/ProgressReporterConfiguration.h"
#include "SEMain/ProgressLogger.h"
#include "SEMain/ProgressBar.h"

namespace SExtractor {

ProgressReporterFactory::ProgressReporterFactory(): m_disable_progress_bar{false} {}

void ProgressReporterFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<ProgressReporterConfiguration>();
}

void ProgressReporterFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto progress_config = manager.getConfiguration<ProgressReporterConfiguration>();
  m_min_interval = progress_config.getMinPrintInterval();
  m_disable_progress_bar = progress_config.isProgressBarDisabled();
}

std::shared_ptr<ProgressReporter> ProgressReporterFactory::createProgressReporter() const {
  std::vector<std::string> entries{"Detected", "Deblended", "Measured", "Segmentation"};

  if (::isatty(::fileno(stderr)) && !m_disable_progress_bar) {
    return ProgressBar::getInstance();
  }
  return std::make_shared<ProgressLogger>(m_min_interval);
}

} // end SExtractor
