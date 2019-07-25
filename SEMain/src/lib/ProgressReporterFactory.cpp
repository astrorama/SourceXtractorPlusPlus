#include "SEImplementation/Configuration/OutputConfig.h"
#include "SEMain/ProgressReporterFactory.h"
#include "SEMain/ProgressLogger.h"
#include "SEMain/ProgressNCurses.h"

namespace SExtractor {

namespace po = boost::program_options;

static const std::string PROGRESS_MIN_INTERVAL{"progress-min-interval"};
static const std::string PROGRESS_BAR_DISABLED{"progress-bar-disable"};

ProgressReporterFactory::ProgressReporterFactory(): m_disable_progress_bar{false} {}

void ProgressReporterFactory::addOptions(boost::program_options::options_description& options) const {
  options.add_options() (PROGRESS_MIN_INTERVAL.c_str(), po::value<int>()->default_value(5),
     "Minimal interval to wait before printing a new log entry with the progress report");
  options.add_options() (PROGRESS_BAR_DISABLED.c_str(), po::bool_switch(),
          "Disable progress bar display");
}

void ProgressReporterFactory::configure(const std::map<std::string, boost::program_options::variable_value> &args) {
  m_min_interval = std::chrono::seconds(args.at(PROGRESS_MIN_INTERVAL).as<int>());
  m_disable_progress_bar = args.at(PROGRESS_BAR_DISABLED).as<bool>();
  // If the output is written to stdout, we can't use the terminal for the fancy ncurses interface
  if (args.at("output-catalog-filename").as<std::string>().empty()) {
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
#endif
  // Always register the logger
  auto logger = std::make_shared<ProgressLogger>(m_min_interval);
  mediator->ProgressObservable::addObserver(logger);
  mediator->DoneObservable::addObserver(logger);
  return mediator;
}

} // end SExtractor
