#include "SEFramework/Task/TaskFactory.h"
#include "SEImplementation/Plugin/ExamplePluginWithConf/ExampleConfWithConf.h"
#include "SEImplementation/Plugin/ExamplePluginWithConf/ExampleSourceTaskWithConf.h"
namespace SExtractor {
class ExampleTaskFactoryWithConf: public TaskFactory {
public:
  virtual ~ExampleTaskFactoryWithConf () = default;
  void reportConfigDependencies (Euclid::Configuration::ConfigManager& manager) const {
    manager.registerConfiguration<ExampleConfWithConf>();
  }
  void configure (Euclid::Configuration::ConfigManager& manager) {
    m_example_string_option = manager.getConfiguration<ExampleConfWithConf>().getExampleStringOption();
    m_example_double_option = manager.getConfiguration<ExampleConfWithConf>().getExampleDoubleOption();
  }
  virtual std::shared_ptr<Task> createTask (const PropertyId& property_id) const {
    return std::make_shared<ExampleSourceTaskWithConf>(m_example_string_option, m_example_double_option);
  }
private:
  std::string m_example_string_option;
  double m_example_double_option;
};
} // namespace SExtractor
