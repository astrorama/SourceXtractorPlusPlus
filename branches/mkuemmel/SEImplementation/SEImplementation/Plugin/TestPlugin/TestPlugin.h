#include "SEFramework/Plugin/Plugin.h"
#include "SEImplementation/Plugin/TestPlugin/TestProperty.h"
#include "SEImplementation/Plugin/TestPlugin/TestTaskFactory.h"
namespace SExtractor {
class TestPlugin : public Plugin {
public:
  virtual ~TestPlugin() = default;
  virtual void registerPlugin(PluginAPI& plugin_api) {
    plugin_api.getTaskFactoryRegistry().registerTaskFactory<TestTaskFactory, TestProperty>();
    plugin_api.getOutputRegistry().registerColumnConverter<TestProperty, int>(
              "test_int",
              [](const TestProperty & prop){
                return prop.getTestInt();
              }
      );

    // Possibly register several more columns for that property
    //
    //plugin_api.getOutputRegistry().registerColumnConverter<ExamplePropertyWithConf, double>(
    //        "scaled_pixel_mean",
    //        [](const ExamplePropertyWithConf& prop){
    //          return prop.getScaledMean();
    //        }
    //);

    plugin_api.getOutputRegistry().optionalOutput<TestProperty>("TestProperty");
  }
  virtual std::string getIdString() const {
    return "test_plugin";
  }
private:
}; // End of TestPlugin class
}
