#include "SEFramework/Task/TaskFactory.h"
#include "SEImplementation/Plugin/ExamplePlugin/ExampleSourceTask.h"
namespace SExtractor {
class ExampleTaskFactory : public TaskFactory {
public:
  ExampleTaskFactory() {}
  virtual ~ExampleTaskFactory() = default;
  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const {
    return std::make_shared<ExampleSourceTask>();
  }
};
} // namespace SExtractor
