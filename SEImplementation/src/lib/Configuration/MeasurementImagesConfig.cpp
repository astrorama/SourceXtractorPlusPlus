/*
 * MeasurementImagesConfig.cpp
 *
 *  Created on: Nov 2, 2016
 *      Author: mschefer
 */

/*
#include "SEFramework/Image/FitsReader.h"
#include "SEImplementation/Configuration/MeasurementConfig.h"
#include "SEImplementation/Configuration/MeasurementImagesConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string MEASUREMENT_IMAGE { "measurement-image" };

MeasurementImagesConfig::MeasurementImagesConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<MeasurementConfig>();
}

std::map<std::string, Configuration::OptionDescriptionList> MeasurementImagesConfig::getProgramOptions() {
  return { {"Measurement image", {
      { MEASUREMENT_IMAGE.c_str(), po::value<std::vector<std::string>>()->default_value({}, ""),
          "Path to a fits format image to be used as measurement image." }
  }}};
}

void MeasurementImagesConfig::initialize(const UserValues& args) {
  auto measurement_image_filenames = args.find(MEASUREMENT_IMAGE)->second.as<std::vector<std::string>>();
  for (auto& filename : measurement_image_filenames) {
    m_measurement_images.push_back(FitsReader<DetectionImage::PixelType>::readFile(filename));
  }
}

const std::vector<std::shared_ptr<MeasurementImage>>& MeasurementImagesConfig::getMeasurementImages() const {
  if (getCurrentState() < State::FINAL) {
    throw Elements::Exception() << "getMeasurementImages() call on not finalized DetectionImageConfig";
  }

  return m_measurement_images;
}

}
*/
