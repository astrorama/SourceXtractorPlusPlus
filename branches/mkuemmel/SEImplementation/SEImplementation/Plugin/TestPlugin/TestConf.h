#include "Configuration/Configuration.h"
namespace po = boost::program_options;
namespace {
const std::string EXAMPLE_INT_OPTION {"test-int-option"};
}
namespace SExtractor {
class TestConf : public Euclid::Configuration::Configuration {
public:
  virtual ~TestConf() = default;
  TestConf(long manager_id) : Configuration(manager_id) {}
  std::map<std::string, OptionDescriptionList> getProgramOptions() {
    return { {"Test option", {
           { EXAMPLE_INT_OPTION.c_str(), po::value<int>()->default_value(1),
                          "An integer option used for the TestPlugin"},
           //{ EXAMPLE_STRING_OPTION.c_str(), po::value<std::string>()->default_value("None"),
           //  "A string option used as an illustration in ExamplePluginWithConf"},
         }}};
  }
  void initialize(const UserValues& args) {
    m_test_int_option = args.at(EXAMPLE_INT_OPTION).as<int>();
    //m_example_string_option = args.at(EXAMPLE_STRING_OPTION).as<std::string>();
  }
  int getTestIntOption () const {
    return m_test_int_option;
  }
  //const std::string& getExampleStringOption () const {
  //  return m_example_string_option;
  //}
private:
  int m_test_int_option;
   //std::string m_example_string_option;
}; // End of ExampleConfWithConf class
} // namespace SExtractor
