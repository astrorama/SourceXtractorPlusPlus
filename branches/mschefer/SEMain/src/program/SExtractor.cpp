/**
 * @file src/program/SExtractor.cpp
 * @date 05/31/16
 * @author mschefer
 */

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include <boost/algorithm/string/predicate.hpp>

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
#include "SEImplementation/Grouping/SplitSourcesCriteria.h"
#include "SEImplementation/Deblending/DeblendingFactory.h"

#include "SEImplementation/SEImplementation/Background/BackgroundLevelAnalyzerFactory.h"
#include "SEImplementation/SEImplementation/Background/BackgroundRMSAnalyzerFactory.h"

#include "SEImplementation/Configuration/DetectionImageConfig.h"
#include "SEImplementation/Configuration/BackgroundConfig.h"
#include "SEImplementation/Configuration/WeightImageConfig.h"
#include "SEImplementation/Configuration/SegmentationConfig.h"

#include "SEMain/SExtractorConfig.h"

#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"
#include "SEMain/PluginConfig.h"

namespace po = boost::program_options;
using namespace SExtractor;
using namespace Euclid::Configuration;

static long config_manager_id = getUniqueManagerId();

static const std::string LIST_OUTPUT_PROPERTIES {"list-output-properties"};

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
  BackgroundLevelAnalyzerFactory background_level_analyzer_factory {};
  BackgroundRMSAnalyzerFactory background_rms_analyzer_factory {};

public:
  
  SEMain(const std::string& plugin_path, const std::vector<std::string>& plugin_list)
          : plugin_manager { task_factory_registry, output_registry, config_manager_id, plugin_path, plugin_list } {
  }

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<SExtractorConfig>();
    config_manager.registerConfiguration<BackgroundConfig>();

    //plugins need to be registered before reportConfigDependencies()
    plugin_manager.loadPlugins();
    task_factory_registry->reportConfigDependencies(config_manager);
    segmentation_factory.reportConfigDependencies(config_manager);
    partition_factory.reportConfigDependencies(config_manager);
    deblending_factory.reportConfigDependencies(config_manager);
    output_factory.reportConfigDependencies(config_manager);
    background_level_analyzer_factory.reportConfigDependencies(config_manager);
    background_rms_analyzer_factory.reportConfigDependencies(config_manager);

    auto options = config_manager.closeRegistration();
    options.add_options() (LIST_OUTPUT_PROPERTIES.c_str(), po::bool_switch(),
          "List the possible output properties for the given input parameters and exit");
    return options;
  }


  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {
    
    // If the user just requested to see the possible output columns we show
    // them and we do nothing else
    
    if (args.at(LIST_OUTPUT_PROPERTIES).as<bool>()) {
      for (auto& name : output_registry->getOptionalOutputNames()) {
        std::cout << name << '\n';
      }
      return Elements::ExitCode::OK;
    }

    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);

    task_factory_registry->configure(config_manager);
    task_factory_registry->registerPropertyInstances(*output_registry);
    
    segmentation_factory.configure(config_manager);
    partition_factory.configure(config_manager);
    deblending_factory.configure(config_manager);
    output_factory.configure(config_manager);
    background_level_analyzer_factory.configure(config_manager);
    background_rms_analyzer_factory.configure(config_manager);

    auto detection_image = config_manager.getConfiguration<DetectionImageConfig>().getDetectionImage();
    auto weight_image = config_manager.getConfiguration<WeightImageConfig>().getWeightImage();
    bool is_weight_absolute = config_manager.getConfiguration<WeightImageConfig>().isWeightAbsolute();
    auto weight_threshold = config_manager.getConfiguration<WeightImageConfig>().getWeightThreshold();
    auto detection_image_coordinate_system = config_manager.getConfiguration<DetectionImageConfig>().getCoordinateSystem();

    auto segmentation = segmentation_factory.createSegmentation();
    auto partition = partition_factory.getPartition();
    auto source_grouping = std::make_shared<SourceGrouping>(
        std::unique_ptr<SplitSourcesCriteria>(new SplitSourcesCriteria), group_factory);
    std::shared_ptr<Deblending> deblending = std::move(deblending_factory.createDeblending());
    std::shared_ptr<Output> output = output_factory.getOutput();

    // Link together the pipeline's steps
    segmentation->addObserver(partition);
    partition->addObserver(source_grouping);
    source_grouping->addObserver(deblending);
    deblending->addObserver(output);

    auto detection_frame = std::make_shared<DetectionImageFrame>(detection_image, weight_image, is_weight_absolute,
        weight_threshold, detection_image_coordinate_system);

    auto background_level_analyzer = background_level_analyzer_factory.createBackgroundAnalyzer();
    background_level_analyzer->analyzeBackground(detection_frame);

    auto background_rms_analyzer = background_rms_analyzer_factory.createBackgroundAnalyzer();
    background_rms_analyzer->analyzeBackground(detection_frame);

    std::cout << "Detected background level: " <<  detection_frame->getBackgroundLevel()
        << " RMS: " << detection_frame->getBackgroundRMS()
        << " threshold: "  << detection_frame->getDetectionThreshold() << '\n';

    const auto& background_config = config_manager.getConfiguration<BackgroundConfig>();

    // Override background level and threshold if requested by the user
    if (background_config.isBackgroundLevelAbsolute()) {
      detection_frame->setBackgroundLevel(background_config.getBackgroundLevel());
    }

    if (background_config.isDetectionThresholdAbsolute()) {
      detection_frame->setDetectionThreshold(background_config.getDetectionThreshold());
    }

    std::cout << "Using background level: " <<  detection_frame->getBackgroundLevel()
          << " RMS: " << detection_frame->getBackgroundRMS()
          << " threshold: "  << detection_frame->getDetectionThreshold() << '\n';

    // Process the image
    segmentation->processFrame(detection_frame);

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
    // The following will consume any extra options in the configuration file
    options.add_options()("*", po::value<std::string>());
    return options;
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
  // to handle any other options except of the plugin-directory and plugin, so
  // we create a subset of the given options with only the necessary ones. We
  // also turn off the the logging.
  std::vector<int> masked_indices {};
  std::vector<std::string> plugin_options_input {};
  plugin_options_input.emplace_back("DummyProgram");
  plugin_options_input.emplace_back("--log-level");
  plugin_options_input.emplace_back("FATAL");
  for (int i = 0; i < argc; ++i) {
    std::string option {argv[i]};
    if (option == "--config-file") {
      plugin_options_input.emplace_back("--config-file");
      plugin_options_input.emplace_back(std::string{argv[i+1]});
    }
    if (boost::starts_with(option, "--config-file=")) {
      plugin_options_input.emplace_back(option);
    }
    if (option == "--plugin-directory") {
      plugin_options_input.emplace_back("--plugin-directory");
      plugin_options_input.emplace_back(std::string{argv[i+1]});
    }
    if (boost::starts_with(option, "--plugin-directory=")) {
      plugin_options_input.emplace_back(option);
    }
    if (option == "--plugin") {
      plugin_options_input.emplace_back("--plugin");
      plugin_options_input.emplace_back(std::string{argv[i+1]});
    }
    if (boost::starts_with(option, "--plugin=")) {
      plugin_options_input.emplace_back(option);
    }
  }
  
  int argc_tmp = plugin_options_input.size();
  std::vector<const char*> argv_tmp (argc_tmp);
  for (unsigned int i=0; i<plugin_options_input.size(); ++i){
    auto& option_str = plugin_options_input[i];
    argv_tmp[i] = option_str.data();
  }

  std::unique_ptr<Elements::Program> plugin_options_main {new PluginOptionsMain{plugin_path, plugin_list}};
  Elements::ProgramManager plugin_options_program {std::move(plugin_options_main),
          THIS_PROJECT_VERSION_STRING, THIS_PROJECT_NAME_STRING};
  plugin_options_program.run(argc_tmp, const_cast<char**>(argv_tmp.data()));
  
  Elements::ProgramManager man {std::unique_ptr<Elements::Program>{new SEMain{plugin_path, plugin_list}},
            THIS_PROJECT_VERSION_STRING, THIS_PROJECT_NAME_STRING};
  Elements::ExitCode exit_code = man.run(argc, argv);
  return static_cast<Elements::ExitCodeType>(exit_code);
}
