#include "SEImplementation/Configuration/OutputConfig.h"
#include "SEMain/ProgressReporterFactory.h"
#include "SEMain/ProgressReporterConfiguration.h"
#include "SEMain/ProgressLogger.h"
#include "SEMain/ProgressNCurses.h"

namespace SExtractor {

ProgressReporterFactory::ProgressReporterFactory(): m_disable_progress_bar{false}, m_log_file{false} {}

void ProgressReporterFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<ProgressReporterConfiguration>();
  manager.registerConfiguration<OutputConfig>();
}

void ProgressReporterFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto progress_config = manager.getConfiguration<ProgressReporterConfiguration>();
  auto output_config = manager.getConfiguration<OutputConfig>();
  m_min_interval = progress_config.getMinPrintInterval();
  m_disable_progress_bar = progress_config.isProgressBarDisabled();
  m_log_file = progress_config.isLogFileSet();
  // If the output is written to stdout, we can't use the terminal for the fancy ncurses interface
  if (output_config.getOutputFile().empty()) {
    m_disable_progress_bar = true;
  }
}

std::shared_ptr<ProgressMediator> ProgressReporterFactory::createProgressMediator() const {
  auto mediator = std::make_shared<ProgressMediator>();

#ifndef WITHOUT_NCURSES
  if (!m_disable_progress_bar && ProgressNCurses::isTerminalCapable()) {
    auto progress_bar = std::make_shared<ProgressNCurses>();
    mediator->ProgressObservable::addObserver(progress_bar);
    mediator->DoneObservable::addObserver(progress_bar);
  }
  if (!::isatty(::fileno(stderr)) || m_log_file || m_disable_progress_bar) {
#endif
    auto logger = std::make_shared<ProgressLogger>(m_min_interval);
    mediator->ProgressObservable::addObserver(logger);
    mediator->DoneObservable::addObserver(logger);
#ifndef WITHOUT_NCURSES
  }
#endif
  mediator->update();
  return mediator;
}

} // end SExtractor
