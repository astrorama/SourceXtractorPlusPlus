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
 * @file src/program/BenchBackgroundModel.cpp
 * @date 30/01/2020
 * @author aalvarez
 */

#include <boost/algorithm/string.hpp>
#include <boost/timer/timer.hpp>

#include <ElementsKernel/Exception.h>
#include <ElementsKernel/Logging.h>
#include <ElementsKernel/Program.h>
#include <ElementsKernel/Main.h>
#include <Configuration/ConfigManager.h>
#include <Configuration/Utils.h>
#include <AlexandriaKernel/memory_tools.h>
#include <AlexandriaKernel/StringUtils.h>

#include "SEFramework/Image/TileManager.h"
#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/FITS/FitsWriter.h"
#include "SEImplementation/Background/SimpleBackgroundAnalyzer.h"
#include "SEImplementation/Background/SE2/SE2BackgroundLevelAnalyzer.h"
#include "SEImplementation/Background/SE/SEBackgroundLevelAnalyzer.h"
#include "SEImplementation/Configuration/DetectionImageConfig.h"
#include "SEImplementation/Configuration/WeightImageConfig.h"


namespace po = boost::program_options;
namespace timer = boost::timer;
using namespace Euclid;
using namespace SourceXtractor;

static long config_manager_id = Configuration::getUniqueManagerId();
static Elements::Logging logger = Elements::Logging::getLogger("BenchConvolution");

/**
 * @class BenchBackgroundModel
 * Run the background modeling over an image, timing it
 */
class BenchBackgroundModel : public Elements::Program {
private:
  DetectionImageConfig m_detection_config;
  WeightImageConfig m_weight_config;

  std::string m_output_bg, m_output_var;
  std::vector<int> m_cell_size, m_smooth;

  enum class Algorithm {
    SIMPLE, SE2, NG
  } m_algorithm;

  std::map<std::string, Algorithm> s_algo_map{
    {"simple", Algorithm::SIMPLE},
    {"se2",    Algorithm::SE2},
    {"ng",     Algorithm::NG}
  };

  /**
   * Get an output path from the input arguments, if defined. Generate a default otherwise.
   */
  std::string getOutputPath(const std::map<std::string, po::variable_value>& args, const std::string& var,
                            const std::string& default_suffix) {
    auto i = args.find(var);
    if (i != args.end())
      return i->second.as<std::string>();
    auto input = boost::filesystem::path(m_detection_config.getDetectionImagePath());
    auto basename = input.leaf();
    while (!basename.extension().empty())
      basename = basename.stem();
    auto algo = args.at("algorithm").as<std::string>();
    auto output_area = boost::filesystem::path(args.at("output-area").as<std::string>());
    auto output = output_area / boost::filesystem::path(basename.native() + "_" + algo + "_" + default_suffix);
    output.replace_extension("fits");
    return output.native();
  }

  std::unique_ptr<BackgroundAnalyzer>
  getBackgroundAnalyzer() {
    switch (m_algorithm) {
      case Algorithm::SIMPLE:
        return make_unique<SimpleBackgroundAnalyzer>();
      case Algorithm::SE2:
        return make_unique<SE2BackgroundLevelAnalyzer>(m_cell_size, m_smooth, m_weight_config.getWeightType());
      case Algorithm::NG:
        return make_unique<SEBackgroundLevelAnalyzer>(m_cell_size, m_smooth, m_weight_config.getWeightType());
    }
    return nullptr;
  }

public:
  BenchBackgroundModel() : m_detection_config(config_manager_id), m_weight_config(config_manager_id) {
  }

  po::options_description defineSpecificProgramOptions() override {
    po::options_description options;

    auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<DetectionImageConfig>();
    config_manager.registerConfiguration<WeightImageConfig>();

    options.add(config_manager.closeRegistration());
    options.add_options()
      ("output", po::value<std::string>(), "Output image for the background")
      ("output-variance", po::value<std::string>(), "Output image for the variance")
      ("output-area", po::value<std::string>()->default_value(boost::filesystem::temp_directory_path().native()), "Output area")
      ("algorithm", po::value<std::string>()->required(), "Algorithm to use: Simple, SE2")
      ("cell-size", po::value<std::string>()->default_value("64"), "Cell size for the histogram")
      ("smooth-size", po::value<std::string>()->default_value("3"), "Box size for the median filtering")
      ("no-write", po::bool_switch(), "Do not write the image (skip interpolation)")
      ("tile-size", po::value<int>()->default_value(512), "Tile size")
      ("tile-memory", po::value<int>()->default_value(2048), "Tile memory limit");

    return options;
  }

  void configure(const std::map<std::string, po::variable_value>& args) {
    auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);

    m_detection_config = config_manager.getConfiguration<DetectionImageConfig>();
    m_weight_config = config_manager.getConfiguration<WeightImageConfig>();

    m_output_bg = getOutputPath(args, "output", "background");
    m_output_var = getOutputPath(args, "output-variance", "variance");

    auto algorithm_str = args.at("algorithm").as<std::string>();
    boost::to_lower(algorithm_str);
    if (s_algo_map.count(algorithm_str) > 0)
      m_algorithm = s_algo_map[algorithm_str];
    else
      throw Elements::Exception() << "Unknown algorithm " << algorithm_str;

    m_cell_size = stringToVector<int>(args.at("cell-size").as<std::string>());
    m_smooth = stringToVector<int>(args.at("smooth-size").as<std::string>());

    auto tile_size = args.at("tile-size").as<int>();
    auto tile_memory = args.at("tile-memory").as<int>();
    TileManager::getInstance()->setOptions(tile_size, tile_size, tile_memory);
  }

  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {
    configure(args);

    logger.info() << "Input image: " << m_detection_config.getDetectionImagePath();
    logger.info() << "Destination background image: " << m_output_bg;
    logger.info() << "Destination variance image: " << m_output_var;

    auto bg_analyzer = getBackgroundAnalyzer();
    assert (bg_analyzer != nullptr);

    auto image = m_detection_config.getDetectionImage();
    auto weight_image = m_weight_config.getWeightImage();
    auto threshold = m_weight_config.getWeightThreshold();

    auto mask = ConstantImage<unsigned char>::create(image->getWidth(), image->getHeight(), false);

    logger.info() << "Starting analysis";
    if (weight_image)
      logger.info() << "Weight image " << weight_image->getRepr();
    else
      logger.info() << "No weight image";

    timer::cpu_timer timer;
    auto bg_model = bg_analyzer->analyzeBackground(image, weight_image, mask, threshold);

    if (!args.at("no-write").as<bool>()) {
      logger.info() << "Writing background";
      FitsWriter::writeFile(*bg_model.getLevelMap(), m_output_bg, m_detection_config.getCoordinateSystem());
      logger.info() << "Writing variance map";
      FitsWriter::writeFile(*bg_model.getVarianceMap(), m_output_var, m_detection_config.getCoordinateSystem());
    }

    timer.stop();
    logger.info() << "Done!";
    std::cout << "Scaling factor: " << bg_model.getScalingFactor() << std::endl;
    std::cout << "Elapsed: " << timer.elapsed().wall << std::endl;

    TileManager::getInstance()->saveAllTiles();
    FitsFileManager::getInstance()->closeAllFiles();

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(BenchBackgroundModel)
