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


using namespace std;
using namespace SExtractor;
using namespace Euclid::Configuration;

int main() {
  
  Elements::Logging::setLevel("DEBUG");
  
  auto& config_manager = ConfigManager::getInstance(1);
  config_manager.registerConfiguration<MeasurementImageConfig>();
  config_manager.closeRegistration();
  
  Configuration::UserValues args {};
  args["python-config-file"].value() = string{"/home/nikoapos/temp/test_config.py"};
  
  config_manager.initialize(args);
  
//  auto& py = config_manager.getConfiguration<PythonConfig>().getInterpreter();
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