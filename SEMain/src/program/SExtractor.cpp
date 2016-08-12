/**
 * @file src/program/SExtractor.cpp
 * @date 05/31/16
 * @author mschefer
 */

#include <map>
#include <string>
#include <boost/program_options.hpp>

#include <iomanip>

#include "ElementsKernel/ProgramHeaders.h"

#include "SEFramework/Plugin/PluginManager.h"

#include "SEFramework/Task/TaskProvider.h"
#include "SEFramework/Image/SubtractImage.h"
#include "SEFramework/Pipeline/SourceGrouping.h"
#include "SEFramework/Pipeline/Deblending.h"
#include "SEFramework/Pipeline/Partition.h"
#include "SEFramework/Output/OutputRegistry.h"

#include "SEFramework/Task/TaskFactoryRegistry.h"

#include "SEFramework/Source/SourceWithOnDemandPropertiesFactory.h"
#include "SEFramework/Source/SourceGroupWithOnDemandPropertiesFactory.h"

#include "SEImplementation/Segmentation/SegmentationFactory.h"
#include "SEImplementation/Output/OutputFactory.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"

#include "SEImplementation/Partition/PartitionFactory.h"
#include "SEImplementation/Grouping/OverlappingBoundariesCriteria.h"
#include "SEImplementation/Deblending/DeblendingFactory.h"

#include "SEImplementation/Configuration/DetectionImageConfig.h"
#include "SEImplementation/Configuration/SegmentationConfig.h"

#include "SEMain/SExtractorConfig.h"

#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"
#include "SEMain/PluginConfig.h"

namespace po = boost::program_options;
using namespace SExtractor;
using namespace Euclid::Configuration;

static long config_manager_id = getUniqueManagerId();

class GroupObserver : public Observer<std::shared_ptr<SourceGroupInterface>> {
public:
  virtual void handleMessage(const std::shared_ptr<SourceGroupInterface>& group) override {
      m_list.push_back(group);
  }

  std::list<std::shared_ptr<SourceGroupInterface>> m_list;
};

class SourceObserver : public Observer<std::shared_ptr<SourceWithOnDemandProperties>> {
public:
  virtual void handleMessage(const std::shared_ptr<SourceWithOnDemandProperties>& source) override {
      m_list.push_back(source);
  }

  std::list<std::shared_ptr<SourceWithOnDemandProperties>> m_list;
};

static Elements::Logging logger = Elements::Logging::getLogger("SExtractor");

class SEMain : public Elements::Program {
  
  std::shared_ptr<TaskFactoryRegistry> task_factory_registry = std::make_shared<TaskFactoryRegistry>();
  std::shared_ptr<TaskProvider> task_provider = std::make_shared<TaskProvider>(task_factory_registry);
  std::shared_ptr<OutputRegistry> output_registry = std::make_shared<OutputRegistry>();
  SegmentationFactory segmentation_factory {task_provider};
  OutputFactory output_factory { output_registry };
  PluginManager plugin_manager;
  std::shared_ptr<SourceFactory> source_factory =
      std::make_shared<SourceWithOnDemandPropertiesFactory>(task_provider);
  std::shared_ptr<SourceGroupFactory> group_factory =
          std::make_shared<SourceGroupWithOnDemandPropertiesFactory>(task_provider);
  PartitionFactory partition_factory {source_factory};
  DeblendingFactory deblending_factory {source_factory};

public:
  
  SEMain(const std::string& plugin_path, const std::vector<std::string>& plugin_list)
          : plugin_manager { task_factory_registry, output_registry, config_manager_id, plugin_path, plugin_list } {
  }

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<SExtractorConfig>();

    //plugins need to be registered before reportConfigDependencies()
    plugin_manager.loadPlugins();

    task_factory_registry->reportConfigDependencies(config_manager);
    segmentation_factory.reportConfigDependencies(config_manager);
    partition_factory.reportConfigDependencies(config_manager);
    deblending_factory.reportConfigDependencies(config_manager);
    output_factory.reportConfigDependencies(config_manager);
    return config_manager.closeRegistration();
  }


  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {

    auto& config_manager = ConfigManager::getInstance(config_manager_id);

    config_manager.initialize(args);

    task_factory_registry->configure(config_manager);
    task_factory_registry->registerPropertyInstances(*output_registry);
    
    segmentation_factory.configure(config_manager);
    partition_factory.configure(config_manager);
    deblending_factory.configure(config_manager);
    output_factory.configure(config_manager);
    
    auto source_observer = std::make_shared<SourceObserver>();
    auto group_observer = std::make_shared<GroupObserver>();

    auto detection_image = config_manager.getConfiguration<DetectionImageConfig>().getDetectionImage();

    auto segmentation = segmentation_factory.getSegmentation();

    auto partition = partition_factory.getPartition();
    
    auto source_grouping = std::make_shared<SourceGrouping>(
        std::unique_ptr<OverlappingBoundariesCriteria>(new OverlappingBoundariesCriteria), group_factory);
    
    auto deblending = deblending_factory.getDeblending();

    std::shared_ptr<Output> output = output_factory.getOutput();

    // Link together the pipeline's steps
    segmentation->addObserver(partition);
    partition->addObserver(source_grouping);
    source_grouping->addObserver(deblending);
    deblending->addObserver(output);

    // Process the image
    segmentation->scan(*detection_image);

    SelectAllCriteria select_all_criteria;
    source_grouping->handleMessage(ProcessSourcesEvent(select_all_criteria));

    return Elements::ExitCode::OK;
  }

};


class PluginOptionsMain : public Elements::Program {
  
public:
  PluginOptionsMain(std::string& plugin_path, std::vector<std::string>& plugin_list) :
          m_plugin_path(plugin_path), m_plugin_list(plugin_list) {
  }
  
  virtual ~PluginOptionsMain() = default;

  boost::program_options::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(conf_man_id);
    config_manager.registerConfiguration<PluginConfig>();
    auto options = config_manager.closeRegistration();
    options.add_options()("*", po::bool_switch());
    return options;
  }

  std::pair<po::options_description, po::positional_options_description> defineProgramArguments() override {
    po::options_description desc("t");
    desc.add_options()("t", po::value<std::vector<std::string>>(), "t");
    po::positional_options_description pos_desc;
    pos_desc.add("t", -1);
    return std::make_pair(desc, pos_desc);
  }
  

  
  Elements::ExitCode mainMethod(std::map<std::string, boost::program_options::variable_value>& args) override {
    auto& config_manager = ConfigManager::getInstance(conf_man_id);
    config_manager.initialize(args);
    auto& conf = config_manager.getConfiguration<PluginConfig>();
    m_plugin_path = conf.getPluginPath();
    m_plugin_list = conf.getPluginList();
    return Elements::ExitCode::OK;
  }

private:
  
  long conf_man_id = getUniqueManagerId();
  std::string& m_plugin_path;
  std::vector<std::string>& m_plugin_list;
  
};



ELEMENTS_API int main(int argc, char* argv[]) {
  std::string plugin_path {};
  std::vector<std::string> plugin_list {};
  
  // First we create a program which has a sole purpose to get the options for
  // the plugin paths. Note that we do not want to have this helper program
  // showing the help message, so if the user gave the --help option we have to
  // mask it
  int help_index = 0;
  for (; help_index < argc; ++help_index) {
    if (std::string{argv[help_index]} == "--help") {
      argv[help_index][0] = ' ';
      break;
    }
  }
  std::set<std::string> args_str (argv, argv + argc);
  if (args_str.count("--help") == 0) {
    std::unique_ptr<Elements::Program> plugin_options_main {new PluginOptionsMain{plugin_path, plugin_list}};
    Elements::ProgramManager plugin_options_program {std::move(plugin_options_main),
            THIS_PROJECT_VERSION_STRING, THIS_PROJECT_NAME_STRING};
    plugin_options_program.run(argc, argv);
  }
  if (help_index < argc) {
    // If we have masked the --help option set it back
    argv[help_index][0] = '-';
  }
  
  Elements::ProgramManager man {std::unique_ptr<Elements::Program>{new SEMain{plugin_path, plugin_list}},
            THIS_PROJECT_VERSION_STRING, THIS_PROJECT_NAME_STRING};
  Elements::ExitCode exit_code = man.run(argc, argv);
  return static_cast<Elements::ExitCodeType>(exit_code);
}
