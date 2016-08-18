/**
 * @file tests/src/TaskProvider_test.cpp
 * @date 05/09/16
 * @author mschefer
 */

#include <memory>

#include <boost/test/unit_test.hpp>

#include "SEFramework/Task/TaskProvider.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Property/Property.h"
#include "SEFramework/Property/Property.h"


using namespace SExtractor;

class ExampleProperty : public Property {
};
class ExamplePropertyB : public Property {
};

class ExampleTask : public SourceTask {
  virtual void computeProperties(SourceInterface& ) const override {
    // Don't do anything
  }
};

// Example implementation of a TaskFactory
class ExampleTaskFactory : public TaskFactory {

  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override {
    // check that we request the correct type of Property
    BOOST_CHECK(property_id == PropertyId::create<ExampleProperty>());

    // And then create a new Task
    return std::make_shared<ExampleTask>();
  }

};

struct TaskProviderFixture {
  std::unique_ptr<ExampleTaskFactory> factory;
  std::shared_ptr<TaskFactoryRegistry> registry;
  std::shared_ptr<TaskProvider> provider;

  TaskProviderFixture()
    : factory(new ExampleTaskFactory()),
      registry(new TaskFactoryRegistry),
      provider(new TaskProvider(registry)) {
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (TaskProvider_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( TaskProvider_test, TaskProviderFixture ) {
  // Register our factory
  registry->registerTaskFactory<ExampleProperty>(std::move(factory));

  auto task = provider->getTask<SourceTask>(PropertyId::create<ExampleProperty>());
  BOOST_CHECK(task);

  // check the task is of the correct type (for testing, normally we would not care about the type)
  auto example_task = std::dynamic_pointer_cast<const ExampleTask>(task);
  BOOST_CHECK(example_task);
}

BOOST_FIXTURE_TEST_CASE( TaskProvider_notfound_test, TaskProviderFixture ) {
  registry->registerTaskFactory<ExampleProperty>(std::move(factory));

  // Try to create a task for property type not registered
  BOOST_CHECK_THROW(provider->getTask<SourceTask>(PropertyId::create<ExamplePropertyB>()), std::exception);
}

BOOST_FIXTURE_TEST_CASE( TaskProvider_duplicate_property, TaskProviderFixture ) {
  // Register our factory
  registry->registerTaskFactory<ExampleProperty>(std::move(factory));

  // Create a second instance of the same registry
  std::unique_ptr<ExampleTaskFactory> same_factory(new ExampleTaskFactory());

  // And then  we try to register it, we should get an exception
  BOOST_CHECK_THROW(registry->registerTaskFactory<ExampleProperty>(std::move(same_factory)), TaskFactoryRegistry::DuplicateFactoryException);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

