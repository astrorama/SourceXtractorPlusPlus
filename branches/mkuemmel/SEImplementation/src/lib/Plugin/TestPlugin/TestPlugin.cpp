#include "SEFramework/Plugin/StaticPlugin.h"
#include "SEImplementation/Plugin/TestPlugin/TestPlugin.h"
namespace SExtractor {
  static StaticPlugin<TestPlugin> test_plugin;
}
