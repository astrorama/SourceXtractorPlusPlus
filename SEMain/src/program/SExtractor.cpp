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

#include "SEFramework/Task/TaskRegistry.h"
#include "SEFramework/Image/SubtractImage.h"
#include "SEFramework/Pipeline/SourceGrouping.h"
#include "SEFramework/Pipeline/Deblending.h"
#include "SEFramework/Pipeline/Partition.h"

#include "SEImplementation/Segmentation/SegmentationFactory.h"

#include "SEImplementation/Property/PixelCentroid.h"

#include "SEImplementation/Task/PixelCentroidTaskFactory.h"
#include "SEImplementation/Task/DetectionFramePixelValuesTaskFactory.h"
#include "SEImplementation/Task/PixelBoundariesTaskFactory.h"
#include "SEImplementation/Task/DetectionFrameSourceStampTaskFactory.h"

#include "SEImplementation/Partition/MinAreaPartitionStep.h"
#include "SEImplementation/Partition/AttractorsPartitionStep.h"
#include "SEImplementation/Grouping/OverlappingBoundariesCriteria.h"
#include "SEImplementation/Grouping/OverlappingBoundariesSourceList.h"

#include "SEConfig/DetectionImageConfig.h"
#include "SEConfig/SegmentationConfig.h"
#include "SEConfig/ExternalFlagConfig.h"

#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"
#include "SEImplementation/Property/ExternalFlag.h"

namespace po = boost::program_options;
using namespace SExtractor;
using namespace Euclid::Configuration;

static long config_manager_id = 0;

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

public:

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<DetectionImageConfig>();
    config_manager.registerConfiguration<SegmentationConfig>();
    config_manager.registerConfiguration<ExternalFlagConfig>();
    return config_manager.closeRegistration();
  }


  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {
    Elements::Logging logger = Elements::Logging::getLogger("SExtractor");

    auto& config_manager = ConfigManager::getInstance(0);
    config_manager.initialize(args);

    auto task_registry = std::make_shared<TaskRegistry>();
    auto source_observer = std::make_shared<SourceObserver>();
    auto group_observer = std::make_shared<GroupObserver>();

    // Register all tasks
    task_registry->registerTaskFactory(std::unique_ptr<PixelCentroidTaskFactory>(new PixelCentroidTaskFactory));
    task_registry->registerTaskFactory(
        std::unique_ptr<DetectionFramePixelValuesTaskFactory>(new DetectionFramePixelValuesTaskFactory));
    task_registry->registerTaskFactory(std::unique_ptr<PixelBoundariesTaskFactory>(new PixelBoundariesTaskFactory));
    task_registry->registerTaskFactory(
        std::unique_ptr<DetectionFrameSourceStampTaskFactory>(new DetectionFrameSourceStampTaskFactory));
    task_registry->registerTaskFactory(std::unique_ptr<TaskFactory>(new ExternalFlagTaskFactory{}));

    auto detection_image = config_manager.getConfiguration<DetectionImageConfig>().getDetectionImage();

    // Segmentation

    auto segmentation = SegmentationFactory(task_registry).getSegmentation();

    auto min_area_step = std::make_shared<MinAreaPartitionStep>(10);
    auto attractors_step = std::make_shared<AttractorsPartitionStep>(task_registry);
    auto partition = std::make_shared<Partition>(std::vector<std::shared_ptr<PartitionStep>>({attractors_step, min_area_step}));

    auto source_grouping = std::make_shared<SourceGrouping>(
        std::unique_ptr<OverlappingBoundariesCriteria>(new OverlappingBoundariesCriteria),
        SourceList::getFactory<OverlappingBoundariesSourceList>());
    auto deblending = std::make_shared<Deblending>(std::vector<std::shared_ptr<DeblendAction>>(), task_registry);

    // Link together the pipeline's steps
    segmentation->addObserver(partition);
    partition->addObserver(source_grouping);
    source_grouping->addObserver(deblending);
    deblending->addObserver(group_observer);

    // Process the image
    segmentation->scan(*detection_image);

    SelectAllCriteria select_all_criteria;
    source_grouping->handleMessage(ProcessSourcesEvent(select_all_criteria));

    std::cout << group_observer->m_list.size() << std::endl;

    std::cout << std::setprecision(10);

    for (auto& group : group_observer->m_list) {
      std::cout << "#" << std::endl;
      for (auto& source : group->getSources()) {
          auto& centroid = source->getProperty<PixelCentroid>();
          std::cout << centroid.getCentroidX() << " / " << centroid.getCentroidY() << std::endl;
          if (args.count("flag-image-top") == 1) {
            auto& flag_info_list = config_manager.getConfiguration<ExternalFlagConfig>().getFlagInfoList();
            auto it = std::find_if(flag_info_list.begin(), flag_info_list.end(),
                    [](const std::pair<std::string, ExternalFlagConfig::FlagInfo>& pair) {
                      return pair.first == "top";
                    });
            auto& top_flag = source->getProperty<ExternalFlag>(it - flag_info_list.begin()); // top
            if (top_flag.getFlag() == 0) {
              std::cout << "TOP\n";
            } else {
              std::cout << "BOTTOM\n";
            }
          }
          if (args.count("flag-image-points") == 1) {
            auto& flag_info_list = config_manager.getConfiguration<ExternalFlagConfig>().getFlagInfoList();
            auto it = std::find_if(flag_info_list.begin(), flag_info_list.end(),
                    [](const std::pair<std::string, ExternalFlagConfig::FlagInfo>& pair) {
                      return pair.first == "points";
                    });
            auto& points_flag = source->getProperty<ExternalFlag>(it - flag_info_list.begin()); // points
            if (points_flag.getFlag() == 1) {
              std::cout << "FLAGGED!!!!\n";
            }
          }
      }
    }

    std::cout << std::endl;

    logger.info("#");
    logger.info("# Exiting mainMethod()");
    logger.info("#");

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(SEMain)



