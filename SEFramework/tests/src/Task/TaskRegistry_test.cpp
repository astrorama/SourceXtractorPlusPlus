/**
 * @file tests/src/TaskRegistry_test.cpp
 * @date 05/09/16
 * @author mschefer
 */

#include <memory>

#include <boost/test/unit_test.hpp>

#include "SEFramework/Task/TaskRegistry.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Property/Property.h"

using namespace SEFramework;

class ExampleProperty : public Property {
};

class ExampleTask : public SourceTask {
  virtual void computeProperties(Source& ) const override {
    // Don't do anything
  }
};

// Example implementation of a TaskFactory
class ExampleTaskFactory : public TaskFactory {

  virtual std::shared_ptr<Task> getTask(PropertyId property_id) override {
    // check that we request the correct type of Property
    BOOST_CHECK(property_id == PropertyId(typeid(ExampleProperty)));

    // And then create a new Task
    return std::make_shared<ExampleTask>();
  }

  virtual const std::vector<PropertyId> getProducedProperties() override  {
    return {typeid(ExampleProperty)};
  }
};

struct TaskRegistryFixture {
  std::unique_ptr<ExampleTaskFactory> factory;
  std::shared_ptr<TaskRegistry> registry;

  TaskRegistryFixture()
    : factory(new ExampleTaskFactory()), registry(new TaskRegistry()) {
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (TaskRegistry_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( taskregistry_test, TaskRegistryFixture ) {
  // Register our factory
  registry->registerTaskFactory(std::move(factory));

  auto task = registry->getTask<SourceTask>(typeid(ExampleProperty));
  BOOST_CHECK(task);

  // check the task is of the correct type (for testing, normally we would not care about the type)
  auto example_task = std::dynamic_pointer_cast<ExampleTask>(task);
  BOOST_CHECK(example_task);
}

BOOST_FIXTURE_TEST_CASE( taskregistry_notfound_test, TaskRegistryFixture ) {
  registry->registerTaskFactory(std::move(factory));

  // Try to create a task for property type not registered
  auto task = registry->getTask<SourceTask>(typeid(Property));

  // Check that we get a null pointer
  BOOST_CHECK(!task);
}

BOOST_FIXTURE_TEST_CASE( taskregistry_duplicate_property, TaskRegistryFixture ) {
  // Register our factory
  registry->registerTaskFactory(std::move(factory));

  // Create a second instance of the same registry
  std::unique_ptr<ExampleTaskFactory> same_factory(new ExampleTaskFactory());

  // And then  we try to register it, we should get an exception
  BOOST_CHECK_THROW(registry->registerTaskFactory(std::move(same_factory)), TaskRegistry::DuplicateFactoryException);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

