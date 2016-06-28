/**
 * @file SEFramework/Registration/RegistrationManager.h
 * @date 06/14/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_REGISTRATION_REGISTRATIONMANAGER_H
#define _SEFRAMEWORK_REGISTRATION_REGISTRATIONMANAGER_H

#include <memory>

#include "SEFramework/Configuration/Configurable.h"

namespace SExtractor {

class TaskRegistry;
class TaskFactory;
class OutputColumn;

/**
 * @class RegistrationManager
 * @brief A singleton class used to register various things globally.
 *
 */
class RegistrationManager : public Configurable {
public:

  virtual ~RegistrationManager();

  // Removes copy/move constructors and assignment
  RegistrationManager(const RegistrationManager&) = delete;
  RegistrationManager(RegistrationManager&&) = delete;
  RegistrationManager& operator=(const RegistrationManager&) = delete;
  RegistrationManager& operator=(RegistrationManager&&) = delete;

  /// Gets a reference to the singleton instance
  static RegistrationManager& instance() {
    // Note: C++11 guarantees a thread-safe initialization
    static RegistrationManager instance;
    return instance;
  }

  /// Used to get a global counter used for PropertyId
  unsigned int getNextPropertyId() {
    return m_property_id_counter++;
  }

  /// Gets a pointer to the TaskRegistry created by RegistrationManager to be used globally
  std::shared_ptr<TaskRegistry> getTaskRegistry() const {
    return m_task_registry;
  }

  /// Registers a TaskFactory, once configured it will be put into the TaskRegistry
  void registerObject(std::unique_ptr<TaskFactory> task_factory);

  void registerOutputColumn(const OutputColumn& output_column);
  const std::map<std::string, OutputColumn>& getOutputColumns() const {
    return m_output_columns;
  }

  // Implements the Configurable interface
  virtual void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) override;
  virtual void configure(Euclid::Configuration::ConfigManager& manager) override;

private:
  RegistrationManager();

  unsigned int m_property_id_counter;
  std::shared_ptr<TaskRegistry> m_task_registry;
  std::vector<std::unique_ptr<TaskFactory>> m_factory_list;
  std::map<std::string, OutputColumn> m_output_columns;

}; /* End of RegistrationManager class */

} /* namespace SExtractor */


#endif
