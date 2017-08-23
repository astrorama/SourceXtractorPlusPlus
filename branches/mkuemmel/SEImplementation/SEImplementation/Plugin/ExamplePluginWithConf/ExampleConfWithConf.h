#include "Configuration/Configuration.h"
namespace po = boost::program_options;
namespace {
const std::string EXAMPLE_DOUBLE_OPTION {"ex-double-option"};
const std::string EXAMPLE_STRING_OPTION {"ex-string-option"};
}
namespace SExtractor {
class ExampleConfWithConf : public Euclid::Configuration::Configuration {
public:
  virtual ~ExampleConfWithConf() = default;
  ExampleConfWithConf(long manager_id) : Configuration(manager_id) {}
  std::map<std::string, OptionDescriptionList> getProgramOptions() {
    return { {"Example options", {
           { EXAMPLE_DOUBLE_OPTION.c_str(), po::value<double>()->default_value(1.0),
                          "A double option used as an illustration in ExamplePluginWithConf"},
           { EXAMPLE_STRING_OPTION.c_str(), po::value<std::string>()->default_value("None"),
             "A string option used as an illustration in ExamplePluginWithConf"},
         }}};
  }
  void initialize(const UserValues& args) {
    m_example_double_option = args.at(EXAMPLE_DOUBLE_OPTION).as<double>();
    m_example_string_option = args.at(EXAMPLE_STRING_OPTION).as<std::string>();
  }
  double getExampleDoubleOption () const {
    return m_example_double_option;
  }
  const std::string& getExampleStringOption () const {
    return m_example_string_option;
  }
private:
  double m_example_double_option;
  std::string m_example_string_option;
}; // End of ExampleConfWithConf class
} // namespace SExtractor
