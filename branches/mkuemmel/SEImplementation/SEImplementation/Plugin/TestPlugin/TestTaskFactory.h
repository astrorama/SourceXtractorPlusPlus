#include "SEFramework/Task/TaskFactory.h"
#include "SEImplementation/Plugin/TestPlugin/TestConf.h"
#include "SEImplementation/Plugin/TestPlugin/TestSourceTask.h"
namespace SExtractor {
class TestTaskFactory: public TaskFactory {
public:
  virtual ~TestTaskFactory () = default;
  void reportConfigDependencies (Euclid::Configuration::ConfigManager& manager) const {
    manager.registerConfiguration<TestConf>();
  }
  void configure (Euclid::Configuration::ConfigManager& manager) {
    //m_example_string_option = manager.getConfiguration<ExampleConfWithConf>().getExampleStringOption();
    m_test_int_option = manager.getConfiguration<TestConf>().getTestIntOption();
  }
  virtual std::shared_ptr<Task> createTask (const PropertyId& property_id) const {
    return std::make_shared<TestSourceTask>(m_test_int_option);
  }
private:
  //std::string m_example_string_option;
  double m_test_int_option;
};
} // namespace SExtractor
