#include "SEFramework/Plugin/Plugin.h"
#include "SEImplementation/Plugin/ExamplePlugin/ExampleProperty.h"
#include "SEImplementation/Plugin/ExamplePlugin/ExampleTaskFactory.h"
namespace SExtractor {
class ExamplePlugin : public Plugin {
public:
  virtual ~ExamplePlugin() = default;
  virtual void registerPlugin(PluginAPI& plugin_api) {
    plugin_api.getTaskFactoryRegistry().registerTaskFactory<ExampleTaskFactory, ExampleProperty>();
    plugin_api.getOutputRegistry().registerColumnConverter<ExampleProperty, double>(
            "pixel_mean_value",
            [](const ExampleProperty& prop){
              return prop.getPixelMeanValue();
            }
    );
    plugin_api.getOutputRegistry().optionalOutput<ExampleProperty>("ExampleProperty");
  }
  virtual std::string getIdString() const {
    return "example_plugin";
  }
private:
}; // End of TestPlugin class
}
