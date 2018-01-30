/**
 * @file src/lib/SegmentationConfig.cpp
 * @date 06/08/16
 * @author mschefer
 */

#include <iostream>
#include <regex>
#include <fstream>
#include <boost/algorithm/string.hpp>

#include "ElementsKernel/Exception.h"

#include "Configuration/ConfigManager.h"
#include "Configuration/CatalogConfig.h"

#include "SEFramework/Image/FitsReader.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEImplementation/Segmentation/BackgroundConvolution.h"

#include "SEImplementation/Configuration/SegmentationConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string SEGMENTATION_ALGORITHM {"segmentation-algorithm" };
static const std::string SEGMENTATION_DISABLE_FILTERING {"segmentation-disable-filtering" };
static const std::string SEGMENTATION_FILTER {"segmentation-filter" };

SegmentationConfig::SegmentationConfig(long manager_id) : Configuration(manager_id),
    m_selected_algorithm(Algorithm::UNKNOWN), m_filtering_enabled(true) {
}

std::map<std::string, Configuration::OptionDescriptionList> SegmentationConfig::getProgramOptions() {
  return { {"Detection image", {
      {SEGMENTATION_ALGORITHM.c_str(), po::value<std::string>()->default_value("LUTZ"),
          "Segmentation algorithm to be used. Currently LUTZ is the only choice"},
      {SEGMENTATION_DISABLE_FILTERING.c_str(), po::bool_switch(),
          "Disables filtering"},
      {SEGMENTATION_FILTER.c_str(), po::value<std::string>()->default_value(""),
          "Loads a filter"},
  }}};
}

void SegmentationConfig::preInitialize(const UserValues& args) {
  auto algorithm_name = boost::to_upper_copy(args.at(SEGMENTATION_ALGORITHM).as<std::string>());
  if (algorithm_name != "LUTZ") {
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
}

void SegmentationConfig::initialize(const UserValues&) {
  m_selected_algorithm = Algorithm::LUTZ;
}

std::shared_ptr<DetectionImageProcessing> SegmentationConfig::getDefaultFilter() const {
  std::cout << "Using the default segmentation filter." << std::endl;
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

std::shared_ptr<DetectionImageProcessing> SegmentationConfig::loadFilter(const std::string& filename) const {
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

std::shared_ptr<DetectionImageProcessing> SegmentationConfig::loadFITSFilter(const std::string& filename) const {

  // read in the FITS file
  auto convolution_kernel = FitsReader<SeFloat>::readFile(filename);

  // give some feedback on the filter
  std::cout << "Loaded segmentation filter: " << filename << " height: " << convolution_kernel->getHeight() << " width: " << convolution_kernel->getWidth() << std::endl;

  // return the correct object
  return std::make_shared<BackgroundConvolution>(convolution_kernel, true);
}

std::shared_ptr<DetectionImageProcessing> SegmentationConfig::loadASCIIFilter(const std::string& filename) const {
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

    // TODO: mandate the # at the beginning of the line
    //if (line.size() == 0 || std::regex_match(line, std::regex("(#)(.*)"))){
    if (line.size() == 0 || std::regex_match(line, std::regex("(#)(.*)"))){
      //std::cout << "skipping line: " << line<< std::endl;
      continue;
    }

    std::stringstream line_stream(line);

    switch (state) {
      case LoadState::STATE_START:
        {
          //std::cout << "STATE_START: " << line<< std::endl;
          std::string conv, norm_type;
          line_stream >> conv >> norm_type;
          if (conv != "CONV") {
            return nullptr;
          }
          if (norm_type == "NORM") {
            normalize = true;
          } else if (norm_type == "NONORM") {
            normalize = false;
          } else {
            return nullptr;
          }
          state = LoadState::STATE_FIRST_LINE;
        }
        break;
      case LoadState::STATE_FIRST_LINE:
        //std::cout << "STATE_FIRST_LINE: " << line<< std::endl;
        while (line_stream.good()) {
          SeFloat value;
          line_stream >> value;
          //std::cout << " value: " << value;
          kernel_data.push_back(value);
        }
        kernel_width = kernel_data.size();
        state = LoadState::STATE_OTHER_LINES;
        break;
      case LoadState::STATE_OTHER_LINES:
        //std::cout << "STATE_OTHER_LINES: " << line<< std::endl;
        while (line_stream.good()) {
          SeFloat value;
          line_stream >> value;
          kernel_data.push_back(value);
        }
        break;
      }
  }

  // compute the dimensions and create the kernel
  auto kernel_height = kernel_data.size() / kernel_width;
  auto convolution_kernel = VectorImage<SeFloat>::create(kernel_width, kernel_height, kernel_data);

  // give some feedback on the filter
  std::cout << "Loaded segmentation filter: " << filename << " height: " << kernel_height << " width: " << kernel_width << std::endl;

  // return the correct object
  return std::make_shared<BackgroundConvolution>(convolution_kernel, normalize);
}

} // SExtractor namespace
