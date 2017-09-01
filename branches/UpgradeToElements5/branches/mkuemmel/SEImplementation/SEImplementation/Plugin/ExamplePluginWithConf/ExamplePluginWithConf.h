#include "SEFramework/Plugin/Plugin.h"
#include "SEImplementation/Plugin/ExamplePluginWithConf/ExamplePropertyWithConf.h"
#include "SEImplementation/Plugin/ExamplePluginWithConf/ExampleTaskFactoryWithConf.h"
namespace SExtractor {
class ExamplePluginWithConf : public Plugin {
public:
  virtual ~ExamplePluginWithConf() = default;
  virtual void registerPlugin(PluginAPI& plugin_api) {
    plugin_api.getTaskFactoryRegistry().registerTaskFactory<ExampleTaskFactoryWithConf, ExamplePropertyWithConf>();
    plugin_api.getOutputRegistry().registerColumnConverter<ExamplePropertyWithConf, std::string>(
              "redondant_name",
              [](const ExamplePropertyWithConf& prop){
                return prop.getName();
              }
      );
    plugin_api.getOutputRegistry().registerColumnConverter<ExamplePropertyWithConf, double>(
            "scaled_pixel_mean",
            [](const ExamplePropertyWithConf& prop){
              return prop.getScaledMean();
            }
    );
    plugin_api.getOutputRegistry().optionalOutput<ExamplePropertyWithConf>("ExamplePropertyWithConf");
  }
  virtual std::string getIdString() const {
    return "example_plugin_with_conf";
  }
private:
}; // End of TestPlugin class
}
