#include "SEFramework/Plugin/Plugin.h"
namespace SExtractor {
class ExamplePluginWithInst : public Plugin {
public:
  virtual ~ExamplePluginWithInst() = default;
  virtual void registerPlugin(PluginAPI& plugin_api) override;
  virtual std::string getIdString() const override;
}; // End of TestPlugin class
}
