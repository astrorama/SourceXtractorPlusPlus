#include <unistd.h>
#include <signal.h>
#include <bits/sigaction.h>
#include "SEMain/ProgressPrinterFactory.h"
#include "SEMain/ProgressPrinterConfiguration.h"
#include "SEMain/ProgressLogger.h"
#include "SEMain/ProgressBar.h"

namespace SExtractor {

static struct sigaction sigint_new, sigint_prev;

static void handleSigint(int s, siginfo_t *info, void *context) {
  // Restore scroll, style and cursor
  std::cerr << "\033[r\033[0\033[?25h";

  // Call the previous handler
  ::sigaction(s, &sigint_prev, nullptr);
  ::raise(s);
}

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
    // C++ guarantees that a static object is initialized once, and in a thread-safe manner
    static struct InstallSigintHandler {
      InstallSigintHandler() {
        ::memset(&sigint_new, 0, sizeof(sigint_new));
        sigint_new.sa_flags = SA_SIGINFO;
        sigint_new.sa_sigaction = handleSigint;
        ::sigaction(SIGINT, &sigint_new, &sigint_prev);
      }
    } _;
    return std::make_shared<ProgressBar>(std::cerr, entries);
  }
  return std::make_shared<ProgressLogger>(m_min_interval, entries);
}

} // end SExtractor
