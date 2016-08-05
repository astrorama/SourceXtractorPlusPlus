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
#include "SEFramework/Registration/OutputRegistry.h"

#include "SEFramework/Task/TaskFactoryRegistry.h"

#include "SEImplementation/Segmentation/SegmentationFactory.h"
#include "SEImplementation/Output/OutputFactory.h"

#include "SEImplementation/Property/PixelCentroid.h"
#include "SEImplementation/Property/ExternalFlag.h"

#include "SEImplementation/Partition/MinAreaPartitionStep.h"
#include "SEImplementation/Partition/AttractorsPartitionStep.h"
#include "SEImplementation/Grouping/OverlappingBoundariesCriteria.h"
#include "SEImplementation/Grouping/OverlappingBoundariesSourceList.h"

#include "SEImplementation/Configuration/DetectionImageConfig.h"
#include "SEImplementation/Configuration/SegmentationConfig.h"
#include "SEImplementation/Configuration/ExternalFlagConfig.h"

#include "SEMain/SExtractorConfig.h"

#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"

namespace po = boost::program_options;
using namespace SExtractor;
using namespace Euclid::Configuration;

static long config_manager_id = getUniqueManagerId();

class GroupObserver : public Observer<std::shared_ptr<EntangledSourceGroup>> {
public:
  virtual void handleMessage(const std::shared_ptr<EntangledSourceGroup>& group) override {
      m_list.push_back(group);
  }

  std::list<std::shared_ptr<EntangledSourceGroup>> m_list;
};

class SourceObserver : public Observer<std::shared_ptr<Source>> {
public:
  virtual void handleMessage(const std::shared_ptr<Source>& source) override {
      m_list.push_back(source);
  }

  std::list<std::shared_ptr<Source>> m_list;
};

static Elements::Logging logger = Elements::Logging::getLogger("SExtractor");

class SEMain : public Elements::Program {
  
  std::shared_ptr<TaskProvider> task_provider = RegistrationManager::instance().getTaskProvider();
  SegmentationFactory segmentation_factory {task_provider};
  OutputFactory output_factory;
  PluginManager plugin_manager;

public:
  
  SEMain() {
  }

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<SExtractorConfig>();

    plugin_manager.reportConfigDependencies(config_manager);

    RegistrationManager::instance().reportConfigDependencies(config_manager);
    segmentation_factory.reportConfigDependencies(config_manager);
    output_factory.reportConfigDependencies(config_manager);
    return config_manager.closeRegistration();
  }


  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {

    auto& config_manager = ConfigManager::getInstance(config_manager_id);

    config_manager.initialize(args);

    plugin_manager.configure(config_manager);

    RegistrationManager::instance().configure(config_manager);
    
    segmentation_factory.configure(config_manager);
    output_factory.configure(config_manager);
    
    auto source_observer = std::make_shared<SourceObserver>();
    auto group_observer = std::make_shared<GroupObserver>();

    auto detection_image = config_manager.getConfiguration<DetectionImageConfig>().getDetectionImage();

    // Segmentation
    auto segmentation = segmentation_factory.getSegmentation();

    auto min_area_step = std::make_shared<MinAreaPartitionStep>(10);
    auto attractors_step = std::make_shared<AttractorsPartitionStep>(task_provider);
    auto partition = std::make_shared<Partition>(std::vector<std::shared_ptr<PartitionStep>>({attractors_step, min_area_step}));

    auto source_grouping = std::make_shared<SourceGrouping>(
        std::unique_ptr<OverlappingBoundariesCriteria>(new OverlappingBoundariesCriteria),
        SourceList::getFactory<OverlappingBoundariesSourceList>());
    auto deblending = std::make_shared<Deblending>(std::vector<std::shared_ptr<DeblendAction>>(), task_provider);

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

MAIN_FOR(SEMain)



