#include "SEFramework/Plugin/StaticPlugin.h"
#include "SEImplementation/Plugin/ExamplePluginWithConf/ExamplePluginWithConf.h"
namespace SExtractor {
  static StaticPlugin<ExamplePluginWithConf> example_plugin_with_conf;
}
