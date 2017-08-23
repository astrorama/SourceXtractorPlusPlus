#include "SEFramework/Plugin/StaticPlugin.h"
#include "SEImplementation/Plugin/ExamplePlugin/ExamplePlugin.h"
namespace SExtractor {
  static StaticPlugin<ExamplePlugin> example_plugin;
}
