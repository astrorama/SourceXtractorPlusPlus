#include "SEFramework/Plugin/StaticPlugin.h"
#include "SEImplementation/Plugin/ExamplePluginWithInst/ExamplePluginWithInst.h"
#include "SEImplementation/Plugin/ExamplePluginWithInst/ExamplePropertyWithInst.h"
#include "SEImplementation/Plugin/ExamplePluginWithInst/ExampleTaskFactoryWithInst.h"
namespace SExtractor {
static StaticPlugin<ExamplePluginWithInst> example_plugin_with_inst;
void ExamplePluginWithInst::registerPlugin (PluginAPI& plugin_api) {
  plugin_api.getTaskFactoryRegistry().registerTaskFactory<ExampleTaskFactoryWithInst, ExamplePropertyWithInst>();
  plugin_api.getOutputRegistry().registerColumnConverter<ExamplePropertyWithInst, double>("FLAG_SUM",
      [](const ExamplePropertyWithInst& prop) {
        return prop.getFlagValueSum();
      });
  plugin_api.getOutputRegistry().enableOutput<ExamplePropertyWithInst>();
}
std::string ExamplePluginWithInst::getIdString () const {
  return "example_plugin_with_inst";
}
}
