/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/**
 * @file src/program/SourceXtractor.cpp
 * @date 05/31/16
 * @author mschefer
 */

#include <typeinfo>
#include <map>
#include <string>
#include <iomanip>

#include <boost/program_options.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <SEMain/SourceXtractorConfig.h>

#include "ElementsKernel/Main.h"
#include "ElementsKernel/System.h"
#include "ElementsKernel/Temporary.h"

#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"

#include "SEFramework/Plugin/PluginManager.h"

#include "SEFramework/Task/TaskProvider.h"
#include "SEFramework/Image/ProcessedImage.h"
#include "SEFramework/Image/BufferedImage.h"
#include "SEFramework/Pipeline/SourceGrouping.h"
#include "SEFramework/Pipeline/Deblending.h"
#include "SEFramework/Pipeline/Partition.h"
#include "SEFramework/Pipeline/Prefetcher.h"
#include "SEFramework/Output/OutputRegistry.h"

#include "SEFramework/Task/TaskFactoryRegistry.h"

#include "SEFramework/Source/SourceWithOnDemandPropertiesFactory.h"
#include "SEFramework/Source/SourceGroupWithOnDemandPropertiesFactory.h"

#include "SEFramework/FITS/FitsFileManager.h"

#include "SEImplementation/CheckImages/SourceIdCheckImage.h"
#include "SEImplementation/CheckImages/DetectionIdCheckImage.h"
#include "SEImplementation/CheckImages/GroupIdCheckImage.h"
#include "SEImplementation/CheckImages/MoffatCheckImage.h"
#include "SEImplementation/Background/BackgroundAnalyzerFactory.h"
#include "SEImplementation/Configuration/MultiThreadingConfig.h"

#include "SEImplementation/Segmentation/SegmentationFactory.h"
#include "SEImplementation/Output/OutputFactory.h"
#include "SEImplementation/Grouping/GroupingFactory.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"

#include "SEImplementation/Partition/PartitionFactory.h"
#include "SEImplementation/Deblending/DeblendingFactory.h"
#include "SEImplementation/Measurement/MeasurementFactory.h"

#include "SEImplementation/Configuration/DetectionImageConfig.h"
#include "SEImplementation/Configuration/BackgroundConfig.h"
#include "SEImplementation/Configuration/SE2BackgroundConfig.h"
#include "SEImplementation/Configuration/WeightImageConfig.h"
#include "SEImplementation/Configuration/MemoryConfig.h"
#include "SEImplementation/Configuration/OutputConfig.h"
#include "SEImplementation/Configuration/SamplingConfig.h"

#include "SEImplementation/CheckImages/CheckImages.h"

#include "SEMain/ProgressReporterFactory.h"
#include "SEMain/PluginConfig.h"
#include "SEMain/Sorter.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;
using namespace SourceXtractor;
using namespace Euclid::Configuration;

static long config_manager_id = getUniqueManagerId();

static const std::string LIST_OUTPUT_PROPERTIES {"list-output-properties"};
static const std::string PROPERTY_COLUMN_MAPPING_ALL {"property-column-mapping-all"};
static const std::string PROPERTY_COLUMN_MAPPING {"property-column-mapping"};
static const std::string DUMP_CONFIG {"dump-default-config"};

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

static Elements::Logging logger = Elements::Logging::getLogger("SourceXtractor");

static void setupEnvironment(void) {
  // Some parts of boost (including boost::filesystem) can throw an exception when the
  // locale as configured in the environment is invalid.
  // We work around that overriding the locale if we find an invalid one.
  // See https://svn.boost.org/trac10/ticket/10205
  try {
    std::locale("");
  }
  catch (...) {
    ::setenv("LC_ALL", "C", 1);
  }
}


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
  GroupingFactory grouping_factory {group_factory};
  DeblendingFactory deblending_factory {source_factory};
  MeasurementFactory measurement_factory { output_registry };
  ProgressReporterFactory progress_printer_factory {};

  bool config_initialized = false;
  po::options_description config_parameters;

public:

  SEMain(const std::string& plugin_path, const std::vector<std::string>& plugin_list)
          : plugin_manager { task_factory_registry, output_registry, config_manager_id, plugin_path, plugin_list } {
  }

  /// Return the options that the underyling configuration register accepts
  /// This is now separated from defineProgramArguments so printDefaults can access these settings,
  /// while ignoring flags that are only used for printing help information (i.e the property list)
  po::options_description getConfigParameters() {
    if (!config_initialized) {
      auto& config_manager = ConfigManager::getInstance(config_manager_id);
      config_manager.registerConfiguration<SourceXtractorConfig>();
      config_manager.registerConfiguration<BackgroundConfig>();
      config_manager.registerConfiguration<SE2BackgroundConfig>();
      config_manager.registerConfiguration<MemoryConfig>();
      config_manager.registerConfiguration<BackgroundAnalyzerFactory>();
      config_manager.registerConfiguration<SamplingConfig>();

      CheckImages::getInstance().reportConfigDependencies(config_manager);

      //plugins need to be registered before reportConfigDependencies()
      plugin_manager.loadPlugins();
      task_factory_registry->reportConfigDependencies(config_manager);
      segmentation_factory.reportConfigDependencies(config_manager);
      partition_factory.reportConfigDependencies(config_manager);
      grouping_factory.reportConfigDependencies(config_manager);
      deblending_factory.reportConfigDependencies(config_manager);
      measurement_factory.reportConfigDependencies(config_manager);
      output_factory.reportConfigDependencies(config_manager);

      config_parameters.add(config_manager.closeRegistration());
      config_initialized = true;
    }
    return config_parameters;
  }

  /// Return the arguments that the program accepts
  std::pair<po::options_description, po::positional_options_description> defineProgramArguments() override {
    auto options = getConfigParameters();

    options.add_options() (LIST_OUTPUT_PROPERTIES.c_str(), po::bool_switch(),
          "List the possible output properties for the given input parameters and exit");
    options.add_options() (PROPERTY_COLUMN_MAPPING_ALL.c_str(), po::bool_switch(),
          "Show the columns created for each property");
    options.add_options() (PROPERTY_COLUMN_MAPPING.c_str(), po::bool_switch(),
          "Show the columns created for each property, for the given configuration");
    options.add_options() (DUMP_CONFIG.c_str(), po::bool_switch(),
                           "Dump parameters with default values into a configuration file");
    progress_printer_factory.addOptions(options);

    // Allow to pass Python options as positional following --
    po::positional_options_description p;
    p.add("python-arg", -1);

    return {options, p};
  }

  /// Print a simple option
  template <typename T>
  static void writeDefault(std::ostream& out, const po::option_description& opt, const boost::any& default_value) {
    out << opt.long_name() << '=' << boost::any_cast<T>(default_value) << std::endl;
  }

  /// Print a multiple-value option
  template <typename T>
  static void writeDefaultMultiple(std::ostream& out, const po::option_description& opt, const boost::any& default_value) {
    auto values = boost::any_cast<std::vector<T>>(default_value);
    if (values.empty()) {
      out << opt.long_name() << '=' << std::endl;
    }
    else {
      for (const auto& v : values)
        out << opt.long_name() << '=' << v << std::endl;
    }
  }

  /// Print a configuration file populated with defaults
  void printDefaults() {
    typedef std::function<void(std::ostream&, const po::option_description&, const boost::any&)> PrinterFunction;
    static std::map<std::type_index, PrinterFunction> printers{
      {typeid(bool), &writeDefault<bool>},
      {typeid(int), &writeDefault<int>},
      {typeid(double), &writeDefault<double>},
      {typeid(std::string), &writeDefault<std::string>},
      {typeid(std::vector<std::string>), &writeDefaultMultiple<std::string>}
    };
    decltype(printers)::const_iterator printer;

    auto config_parameters = getConfigParameters();
    for (const auto& p : config_parameters.options()) {
      boost::any default_value;

      std::cout << "# " << p->description() << std::endl;
      if (!p->semantic()->apply_default(default_value)) {
        std::cout << '#' << p->long_name() << "=" << std::endl;
      }
      else if ((printer = printers.find(default_value.type())) == printers.end()) {
        std::cout << '#' << p->long_name() << "=<Unknown type " << default_value.type().name() << '>' << std::endl;
      }
      else {
        printer->second(std::cout, *p, default_value);
      }
      std::cout << std::endl;
    }

    // We need to print the log options manually, as that is set up by Elements
    std::cout << "# Log level: FATAL, ERROR, WARN, INFO, DEBUG" << std::endl;
    std::cout << "log-level=INFO" << std::endl;
    std::cout << "# Log file" << std::endl;
    std::cout << "#log-file" << std::endl;
  }

  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {

    // If the user just requested to see the possible output columns we show
    // them and we do nothing else

    if (args.at(LIST_OUTPUT_PROPERTIES).as<bool>()) {
      for (auto& name : output_registry->getOutputPropertyNames()) {
        std::cout << name << std::endl;
      }
      return Elements::ExitCode::OK;
    }

    if (args.at(PROPERTY_COLUMN_MAPPING_ALL).as<bool>()) {
      output_registry->printPropertyColumnMap();
      return Elements::ExitCode::OK;
    }

    if (args.at(DUMP_CONFIG).as<bool>()) {
      printDefaults();
      return Elements::ExitCode::OK;
    }

    // Elements does not verify that the config-file exists. It will just not read it.
    // We verify that it does exist here.
    if (args.find("config-file") != args.end()) {
      auto cfg_file = args.at("config-file").as<fs::path>();
      if (cfg_file != "" && !fs::exists(cfg_file)) {
        throw Elements::Exception() << "The configuration file '" << cfg_file << "' does not exist";
      }
    }

    // Create the progress listener and printer ASAP
    progress_printer_factory.configure(args);
    auto progress_mediator = progress_printer_factory.createProgressMediator();

    // Initialize the rest of the components
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);

    // Configure TileManager
    auto memory_config = config_manager.getConfiguration<MemoryConfig>();
    TileManager::getInstance()->setOptions(memory_config.getTileSize(),
        memory_config.getTileSize(), memory_config.getTileMaxMemory());

    CheckImages::getInstance().configure(config_manager);

    task_factory_registry->configure(config_manager);
    task_factory_registry->registerPropertyInstances(*output_registry);

    segmentation_factory.configure(config_manager);
    partition_factory.configure(config_manager);
    grouping_factory.configure(config_manager);
    deblending_factory.configure(config_manager);
    measurement_factory.configure(config_manager);
    output_factory.configure(config_manager);

    if (args.at(PROPERTY_COLUMN_MAPPING).as<bool>()) {
      output_registry->printPropertyColumnMap(config_manager.getConfiguration<OutputConfig>().getOutputProperties());
      return Elements::ExitCode::OK;
    }

    auto detection_image = config_manager.getConfiguration<DetectionImageConfig>().getDetectionImage();
    auto detection_image_path = config_manager.getConfiguration<DetectionImageConfig>().getDetectionImagePath();
    auto weight_image = config_manager.getConfiguration<WeightImageConfig>().getWeightImage();
    bool is_weight_absolute = config_manager.getConfiguration<WeightImageConfig>().isWeightAbsolute();
    auto weight_threshold = config_manager.getConfiguration<WeightImageConfig>().getWeightThreshold();

    auto detection_image_coordinate_system = config_manager.getConfiguration<DetectionImageConfig>().getCoordinateSystem();
    auto detection_image_gain = config_manager.getConfiguration<DetectionImageConfig>().getGain();
    auto detection_image_saturation = config_manager.getConfiguration<DetectionImageConfig>().getSaturation();

    auto segmentation = segmentation_factory.createSegmentation();

    // Prefetcher
    auto multithreading_config = config_manager.getConfiguration<MultiThreadingConfig>();
    auto prefetcher = std::make_shared<Prefetcher>(multithreading_config.getThreadPool());

    // Rest of the stagees
    auto partition = partition_factory.getPartition();
    auto source_grouping = grouping_factory.createGrouping();
    prefetcher->requestProperties(source_grouping->requiredProperties());

    std::shared_ptr<Deblending> deblending = deblending_factory.createDeblending();
    std::shared_ptr<Measurement> measurement = measurement_factory.getMeasurement();
    std::shared_ptr<Output> output = output_factory.getOutput();

    prefetcher->requestProperties(deblending->requiredProperties());

    auto sorter = std::make_shared<Sorter>();

    // Link together the pipeline's steps
    segmentation->Observable<std::shared_ptr<SourceInterface>>::addObserver(partition);
    segmentation->Observable<ProcessSourcesEvent>::addObserver(source_grouping);
    partition->addObserver(prefetcher);
    prefetcher->addObserver(source_grouping);
    source_grouping->addObserver(deblending);
    deblending->addObserver(measurement);
    measurement->addObserver(sorter);
    sorter->addObserver(output);

    segmentation->Observable<SegmentationProgress>::addObserver(progress_mediator->getSegmentationObserver());
    segmentation->Observable<std::shared_ptr<SourceInterface>>::addObserver(progress_mediator->getDetectionObserver());
    deblending->addObserver(progress_mediator->getDeblendingObserver());
    measurement->addObserver(progress_mediator->getMeasurementObserver());

    // Add observers for CheckImages
    if (CheckImages::getInstance().getSegmentationImage() != nullptr) {
      segmentation->Observable<std::shared_ptr<SourceInterface>>::addObserver(
          std::make_shared<DetectionIdCheckImage>());
    }
    if (CheckImages::getInstance().getPartitionImage() != nullptr) {
      measurement->addObserver(
          std::make_shared<SourceIdCheckImage>());
    }
    if (CheckImages::getInstance().getGroupImage() != nullptr) {
      measurement->addObserver(
          std::make_shared<GroupIdCheckImage>());
    }
    if (CheckImages::getInstance().getMoffatImage() != nullptr) {
      measurement->addObserver(
          std::make_shared<MoffatCheckImage>());
    }

    auto interpolation_gap = config_manager.getConfiguration<DetectionImageConfig>().getInterpolationGap();
    auto detection_frame = std::make_shared<DetectionImageFrame>(detection_image, weight_image,
        weight_threshold, detection_image_coordinate_system, detection_image_gain,
        detection_image_saturation, interpolation_gap);
    detection_frame->setLabel(boost::filesystem::basename(detection_image_path));

    auto background_analyzer = config_manager.getConfiguration<BackgroundAnalyzerFactory>().createBackgroundAnalyzer();
    auto background_model = background_analyzer->analyzeBackground(detection_frame->getOriginalImage(), weight_image,
        ConstantImage<unsigned char>::create(detection_image->getWidth(), detection_image->getHeight(), false), detection_frame->getVarianceThreshold());

    // initial set of the variance and background check images, might be overwritten below
    CheckImages::getInstance().setBackgroundCheckImage(background_model.getLevelMap());
    CheckImages::getInstance().setVarianceCheckImage(background_model.getVarianceMap());

    detection_frame->setBackgroundLevel(background_model.getLevelMap(), background_model.getMedianRms());

    if (weight_image != nullptr) {
      if (is_weight_absolute) {
        detection_frame->setVarianceMap(weight_image);
      } else {
        auto scaled_image = MultiplyImage<SeFloat>::create(weight_image, background_model.getScalingFactor());
        detection_frame->setVarianceMap(scaled_image);
      }
    } else {
      detection_frame->setVarianceMap(background_model.getVarianceMap());
    }
    // re-set the variance check image to what's in the detection_frame()
    CheckImages::getInstance().setVarianceCheckImage(detection_frame->getVarianceMap());

    const auto& background_config = config_manager.getConfiguration<BackgroundConfig>();

    // Override background level and threshold if requested by the user
    if (background_config.isBackgroundLevelAbsolute()) {
      auto background = ConstantImage<DetectionImage::PixelType>::create(
          detection_image->getWidth(), detection_image->getHeight(), background_config.getBackgroundLevel());

      detection_frame->setBackgroundLevel(background, 0.);
      CheckImages::getInstance().setBackgroundCheckImage(background);
    }

    if (background_config.isDetectionThresholdAbsolute()) {
      detection_frame->setDetectionThreshold(background_config.getDetectionThreshold());
    }
    CheckImages::getInstance().setVarianceCheckImage(detection_frame->getVarianceMap());

    //CheckImages::getInstance().setFilteredCheckImage(detection_frame->getFilteredImage());

    // Perform measurements (multi-threaded part)
    measurement->startThreads();

    try {
      // Process the image
      segmentation->processFrame(detection_frame);
    }
    catch (const std::exception &e) {
      logger.error() << "Failed to process the frame! " << e.what();
      measurement->waitForThreads();
      return Elements::ExitCode::NOT_OK;
    }

    measurement->waitForThreads();

    CheckImages::getInstance().setFilteredCheckImage(detection_frame->getFilteredImage());
    CheckImages::getInstance().setThresholdedCheckImage(detection_frame->getThresholdedImage());
    CheckImages::getInstance().setSnrCheckImage(detection_frame->getSnrImage());
    CheckImages::getInstance().saveImages();
    TileManager::getInstance()->flush();
    FitsFileManager::getInstance()->closeAllFiles();

    size_t n_writen_rows = output->flush();

    progress_mediator->done();

    if (n_writen_rows > 0) {
      logger.info() << n_writen_rows << " sources detected";
    } else {
      logger.info() << "NO SOURCES DETECTED";
    }

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
    options.add_options()("*", po::value<std::vector<std::string>>());
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


static void forwardOptions(int argc, char *const *argv, std::vector<std::string>& plugin_options_input) {
  for (int i = 0; i < argc; ++i) {
    std::string option{argv[i]};
    if (option == "--config-file") {
      plugin_options_input.emplace_back("--config-file");
      plugin_options_input.emplace_back(std::string{argv[i + 1]});
    }
    if (boost::starts_with(option, "--config-file=")) {
      plugin_options_input.emplace_back(option);
    }
    if (option == "--plugin-directory") {
      plugin_options_input.emplace_back("--plugin-directory");
      plugin_options_input.emplace_back(std::string{argv[i + 1]});
    }
    if (boost::starts_with(option, "--plugin-directory=")) {
      plugin_options_input.emplace_back(option);
    }
    if (option == "--plugin") {
      plugin_options_input.emplace_back("--plugin");
      plugin_options_input.emplace_back(std::string{argv[i + 1]});
    }
    if (boost::starts_with(option, "--plugin=")) {
      plugin_options_input.emplace_back(option);
    }
  }
}


ELEMENTS_API int main(int argc, char* argv[]) {
  std::string plugin_path {};
  std::vector<std::string> plugin_list {};

  // This adds the current directory as a valid location for the default "sourcextractor++.conf" configuration
  Elements::TempEnv local_env;
  if (local_env["ELEMENTS_CONF_PATH"].empty()) {
    local_env["ELEMENTS_CONF_PATH"] = ".:/etc";
  } else {
    local_env["ELEMENTS_CONF_PATH"] = ".:" + local_env["ELEMENTS_CONF_PATH"] + ":/etc";
  }

  setupEnvironment();

  // Try to be reasonably graceful with unhandled exceptions
  std::set_terminate(&Elements::ProgramManager::onTerminate);

  try {
    // First we create a program which has a sole purpose to get the options for
    // the plugin paths. Note that we do not want to have this helper program
    // to handle any other options except of the plugin-directory and plugin, so
    // we create a subset of the given options with only the necessary ones. We
    // also turn off the the logging.
    std::vector<int> masked_indices{};
    std::vector<std::string> plugin_options_input{};
    plugin_options_input.emplace_back("DummyProgram");
    plugin_options_input.emplace_back("--log-level");
    plugin_options_input.emplace_back("ERROR");
    forwardOptions(argc, argv, plugin_options_input);

    int argc_tmp = plugin_options_input.size();
    std::vector<const char *> argv_tmp(argc_tmp);
    for (unsigned int i = 0; i < plugin_options_input.size(); ++i) {
      auto& option_str = plugin_options_input[i];
      argv_tmp[i] = option_str.data();
    }

    CREATE_MANAGER_WITH_ARGS(plugin_options_program, PluginOptionsMain, plugin_path, plugin_list);
    plugin_options_program.run(argc_tmp, const_cast<char **>(argv_tmp.data()));

    CREATE_MANAGER_WITH_ARGS(main, SEMain, plugin_path, plugin_list);
    Elements::ExitCode exit_code = main.run(argc, argv);
    return static_cast<Elements::ExitCodeType>(exit_code);
  }
  catch (const std::exception &e) {
    logger.fatal() << e.what();
    return static_cast<Elements::ExitCodeType>(Elements::ExitCode::NOT_OK);
  }
  catch (...) {
    logger.fatal() << "Unknown exception type!";
    logger.fatal() << "Please, report this as a bug";
    return static_cast<Elements::ExitCodeType>(Elements::ExitCode::SOFTWARE);
  }
}
