#include <memory>
#include "SEFramework/Task/TaskFactory.h"
#include "SEImplementation/Plugin/ExamplePluginWithInst/ExampleConfWithInst.h"
//#include "SEImplementation/Plugin/ExamplePluginWithInst/ExamplePropertyWithInst.h"
#include "SEImplementation/Plugin/ExamplePluginWithInst/ExampleSourceTaskWithInst.h"
namespace SExtractor {
class ExampleTaskFactoryWithInst: public TaskFactory {
public:
  virtual ~ExampleTaskFactoryWithInst () = default;
  void reportConfigDependencies (Euclid::Configuration::ConfigManager& manager) const {
    manager.registerConfiguration<ExampleConfWithInst>();
  }
  void configure (Euclid::Configuration::ConfigManager& manager) {
    auto flag_image_map_from_conf = manager.getConfiguration<ExampleConfWithInst>().getFlagImageMap();
    unsigned int instance_idx = 0;
    for (auto& flag_name_image : flag_image_map_from_conf) {
      m_instance_names.emplace_back(std::make_pair(flag_name_image.first, instance_idx));
      auto property_id = PropertyId::create<ExamplePropertyWithInst>(instance_idx);
      m_flag_image_map[property_id] = flag_name_image.second;
      instance_idx++;
    }
  }
  virtual std::shared_ptr<Task> createTask (const PropertyId& property_id) const {
    if (m_flag_image_map.find(property_id) != m_flag_image_map.end()) {
      auto& flag_image = m_flag_image_map.at(property_id);
      return std::make_shared<ExampleSourceTaskWithInst>(flag_image, property_id.getIndex());
    }
    return nullptr;
  }
  void registerPropertyInstances (OutputRegistry& output_registry) {
      output_registry.registerPropertyInstances<ExamplePropertyWithInst>(m_instance_names);
  }
private:
  std::map<PropertyId, std::shared_ptr<FlagImage>> m_flag_image_map;
  std::vector<std::pair<std::string, unsigned int>> m_instance_names;
};
} // namespace SExtractor
