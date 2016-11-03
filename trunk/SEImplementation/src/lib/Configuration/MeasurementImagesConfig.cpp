/*
 * MeasurementImagesConfig.cpp
 *
 *  Created on: Nov 2, 2016
 *      Author: mschefer
 */


#include "SEImplementation/Configuration/MeasurementImagesConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

MeasurementImagesConfig::MeasurementImagesConfig(long manager_id) : Configuration(manager_id) {
}

std::map<std::string, Configuration::OptionDescriptionList> MeasurementImagesConfig::getProgramOptions() {
  return {};
}

void MeasurementImagesConfig::initialize(const UserValues& args) {

}

}
