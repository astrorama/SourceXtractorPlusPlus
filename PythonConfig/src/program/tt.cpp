/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * @file tt.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <iostream>
#include <PythonConfig/PythonInterpreter.h>
#include <PythonConfig/PyMeasurementImage.h>
#include <PythonConfig/Configuration/PythonConfig.h>
#include <PythonConfig/Configuration/MeasurementImageConfig.h>
#include <Configuration/ConfigManager.h>
#include <ElementsKernel/Logging.h>
#include <PythonConfig/Configuration/ApertureConfig.h>


using namespace std;
using namespace SExtractor;
using namespace Euclid::Configuration;

int main() {

  Elements::Logging::setLevel("DEBUG");

  auto &config_manager = ConfigManager::getInstance(1);
  config_manager.registerConfiguration<MeasurementImageConfig>();
  config_manager.registerConfiguration<ApertureConfig>();
  config_manager.closeRegistration();

  Configuration::UserValues args{};
  args["python-config-file"].value() = (boost::filesystem::path(getenv("HOME")) / "temp/test_config.py").native();

  config_manager.initialize(args);

  auto &py = config_manager.getConfiguration<PythonConfig>().getInterpreter();

  auto &mconfig = config_manager.getConfiguration<MeasurementImageConfig>();
  auto &aconfig = config_manager.getConfiguration<ApertureConfig>();

  auto &measurement_images = mconfig.getImagePaths();

  auto aper_out = aconfig.getOutputForImages();
  for (auto &img_id : aper_out) {
    std::cout  << "Aperture column for image " << measurement_images[img_id] << std::endl;
    auto apertures = aconfig.getAperturesForImage(img_id);
    for (auto a : apertures) {
      std::cout << '\t' << a << std::endl;
    }
  }

//  
//  auto image_map = py.getMeasurementImages();
//  for (auto& p : image_map) {
//    cout << p.first << ": " << p.second.file << '\n';
//  }
//  cout << image_map.size() << '\n';

//  for (auto& psf : config_manager.getConfiguration<MeasurementImageConfig>().getPsfsPaths()) {
//    cout << psf << '\n';
//  }
}
