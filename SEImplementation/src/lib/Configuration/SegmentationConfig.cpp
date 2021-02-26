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
 * @file src/lib/SegmentationConfig.cpp
 * @date 06/08/16
 * @author mschefer
 */

#include <iostream>
#include <fstream>

#include <boost/regex.hpp>
using boost::regex;
using boost::regex_match;
using boost::smatch;

#include <boost/algorithm/string.hpp>

#include "ElementsKernel/Exception.h"

#include "Configuration/ConfigManager.h"

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/FITS/FitsReader.h"

#include "SEImplementation/Segmentation/BackgroundConvolution.h"
#include "SEImplementation/Configuration/SegmentationConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

static Elements::Logging segConfigLogger = Elements::Logging::getLogger("Config");

static const std::string SEGMENTATION_ALGORITHM {"segmentation-algorithm" };
static const std::string SEGMENTATION_DISABLE_FILTERING {"segmentation-disable-filtering" };
static const std::string SEGMENTATION_FILTER {"segmentation-filter" };
static const std::string SEGMENTATION_LUTZ_WINDOW_SIZE {"segmentation-lutz-window-size" };
static const std::string SEGMENTATION_BFS_MAX_DELTA {"segmentation-bfs-max-delta" };
static const std::string SEGMENTATION_ONNX_MODEL {"segmentation-onnx-model" };

SegmentationConfig::SegmentationConfig(long manager_id) : Configuration(manager_id),
    m_selected_algorithm(Algorithm::UNKNOWN), m_lutz_window_size(0), m_bfs_max_delta(1000) {
}

std::map<std::string, Configuration::OptionDescriptionList> SegmentationConfig::getProgramOptions() {
  return { {"Detection image", {
      {SEGMENTATION_ALGORITHM.c_str(), po::value<std::string>()->default_value("LUTZ"),
          "Segmentation algorithm to be used (LUTZ or TILES)"},
      {SEGMENTATION_DISABLE_FILTERING.c_str(), po::bool_switch(),
          "Disables filtering"},
      {SEGMENTATION_FILTER.c_str(), po::value<std::string>()->default_value(""),
          "Loads a filter"},
      {SEGMENTATION_LUTZ_WINDOW_SIZE.c_str(), po::value<int>()->default_value(0),
          "Lutz sliding window size (0=disable)"},
      {SEGMENTATION_BFS_MAX_DELTA.c_str(), po::value<int>()->default_value(1000),
          "BFS algorithm max source x/y size (default=1000)"},
      {SEGMENTATION_ONNX_MODEL.c_str(), po::value<std::string>()->default_value(""),
          "ONNX model to use with machine learning segmentation"},
  }}};
}

void SegmentationConfig::preInitialize(const UserValues& args) {
  auto algorithm_name = boost::to_upper_copy(args.at(SEGMENTATION_ALGORITHM).as<std::string>());
  if (algorithm_name == "LUTZ") {
    m_selected_algorithm = Algorithm::LUTZ;
  } else if (algorithm_name == "BFS") {
    m_selected_algorithm = Algorithm::BFS;
  } else if (algorithm_name == "ML") {
    m_selected_algorithm = Algorithm::ML;
  } else {
    throw Elements::Exception() << "Unknown segmentation algorithm : " << algorithm_name;
  }

  if (args.at(SEGMENTATION_DISABLE_FILTERING).as<bool>()) {
    m_filter = nullptr;
  } else {
    auto filter_filename = args.at(SEGMENTATION_FILTER).as<std::string>();
    if (filter_filename != "") {
      m_filter = loadFilter(filter_filename);
      if (m_filter == nullptr)
        throw Elements::Exception() << "Can not load filter: " << filter_filename;
    } else {
      m_filter = getDefaultFilter();
    }
  }

  m_lutz_window_size = args.at(SEGMENTATION_LUTZ_WINDOW_SIZE).as<int>();
  m_bfs_max_delta = args.at(SEGMENTATION_BFS_MAX_DELTA).as<int>();
  m_onnx_model_path = args.at(SEGMENTATION_ONNX_MODEL).as<std::string>();

  if (m_selected_algorithm == Algorithm::ML && m_onnx_model_path == "") {
    throw Elements::Exception() << "Machine learning segmentation requested but no ONNX model was provided";
  }
}

void SegmentationConfig::initialize(const UserValues&) {
}

std::shared_ptr<DetectionImageFrame::ImageFilter> SegmentationConfig::getDefaultFilter() const {
  segConfigLogger.info() << "Using the default segmentation (3x3) filter.";
  auto convolution_kernel = VectorImage<SeFloat>::create(3, 3);
  convolution_kernel->setValue(0,0, 1);
  convolution_kernel->setValue(0,1, 2);
  convolution_kernel->setValue(0,2, 1);

  convolution_kernel->setValue(1,0, 2);
  convolution_kernel->setValue(1,1, 4);
  convolution_kernel->setValue(1,2, 2);

  convolution_kernel->setValue(2,0, 1);
  convolution_kernel->setValue(2,1, 2);
  convolution_kernel->setValue(2,2, 1);

  return std::make_shared<BackgroundConvolution>(convolution_kernel, true);
}

std::shared_ptr<DetectionImageFrame::ImageFilter> SegmentationConfig::loadFilter(const std::string& filename) const {
  // check for the extension ".fits"
  std::string fits_ending(".fits");
  if (filename.length() >= fits_ending.length()
      && filename.compare (filename.length() - fits_ending.length(), fits_ending.length(), fits_ending)==0) {
    // load a FITS filter
    return loadFITSFilter(filename);
  }
  else{
    // load an ASCII filter
    return loadASCIIFilter(filename);
  }
}

std::shared_ptr<DetectionImageFrame::ImageFilter> SegmentationConfig::loadFITSFilter(const std::string& filename) const {

  // read in the FITS file
  auto convolution_kernel = FitsReader<SeFloat>::readFile(filename);

  // give some feedback on the filter
  segConfigLogger.info() << "Loaded segmentation filter: " << filename << " height: " << convolution_kernel->getHeight() << " width: " << convolution_kernel->getWidth();

  // return the correct object
  return std::make_shared<BackgroundConvolution>(convolution_kernel, true);
}

static bool getNormalization(std::istream& line_stream) {
  std::string conv, norm_type;
  line_stream >> conv >> norm_type;
  if (conv != "CONV") {
    throw Elements::Exception() << "Unexpected start for ASCII filter: " << conv;
  }
  if (norm_type == "NORM") {
    return true;
  }
  else if (norm_type == "NONORM") {
    return false;
  }

  throw Elements::Exception() << "Unexpected normalization type: " << norm_type;
}

template <typename T>
static void extractValues(std::istream& line_stream, std::vector<T>& data) {
  T value;
  while (line_stream.good()) {
    line_stream >> value;
    data.push_back(value);
  }
}

std::shared_ptr<DetectionImageFrame::ImageFilter> SegmentationConfig::loadASCIIFilter(const std::string& filename) const {
  std::ifstream file;

  // open the file and check
  file.open(filename);
  if (!file.good() || !file.is_open()){
    throw Elements::Exception() << "Can not load filter: " << filename;
  }

  enum class LoadState {
    STATE_START,
    STATE_FIRST_LINE,
    STATE_OTHER_LINES
  };

  LoadState state = LoadState::STATE_START;
  bool normalize = false;
  std::vector<SeFloat> kernel_data;
  unsigned int kernel_width = 0;

  while (file.good()) {
    std::string line;
    std::getline(file, line);
    line = regex_replace(line, regex("\\s*#.*"), std::string(""));
    line = regex_replace(line, regex("\\s*$"), std::string(""));
    if (line.size() == 0) {
      continue;
    }

    std::stringstream line_stream(line);

    switch (state) {
      case LoadState::STATE_START:
        normalize = getNormalization(line_stream);
        state = LoadState::STATE_FIRST_LINE;
        break;
      case LoadState::STATE_FIRST_LINE:
        extractValues(line_stream, kernel_data);
        kernel_width = kernel_data.size();
        state = LoadState::STATE_OTHER_LINES;
        break;
      case LoadState::STATE_OTHER_LINES:
        extractValues(line_stream, kernel_data);
        break;
      }
  }

  // compute the dimensions and create the kernel
  auto kernel_height = kernel_data.size() / kernel_width;
  auto convolution_kernel = VectorImage<SeFloat>::create(kernel_width, kernel_height, kernel_data);

  // give some feedback on the filter
  segConfigLogger.info() << "Loaded segmentation filter: " << filename << " width: " << convolution_kernel->getWidth() << " height: " << convolution_kernel->getHeight();

  // return the correct object
  return std::make_shared<BackgroundConvolution>(convolution_kernel, normalize);
}

} // SourceXtractor namespace
