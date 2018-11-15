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
#include <SEImplementation/PythonConfig/PythonInterpreter.h>
#include <SEImplementation/PythonConfig/PyMeasurementImage.h>
#include <SEImplementation/Configuration/PythonConfig.h>
#include <SEImplementation/Configuration/MeasurementImageConfig.h>
#include <Configuration/ConfigManager.h>
#include <ElementsKernel/Logging.h>
#include <SEImplementation/Configuration/ApertureConfig.h>
#include <SEImplementation/Configuration/ModelFittingConfig.h>
#include <SEImplementation/PythonConfig/ObjectInfo.h>
#include <SEFramework/Source/SimpleSourceFactory.h>
#include <SEImplementation/Plugin/WorldCentroid/WorldCentroid.h>
#include <SEImplementation/Plugin/ShapeParameters/ShapeParameters.h>
#include <SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h>
#include <ModelFitting/Engine/EngineParameterManager.h>
#include <SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameterManager.h>
#include "ModelFitting/Parameters/DependentParameter.h"



using namespace std;
using namespace SExtractor;
using namespace Euclid::Configuration;

namespace py = boost::python;

int main() {
  Elements::Logging::setLevel("DEBUG");

  auto &config_manager = ConfigManager::getInstance(1);
  config_manager.registerConfiguration<MeasurementImageConfig>();
  config_manager.registerConfiguration<ApertureConfig>();
  config_manager.registerConfiguration<ModelFittingConfig>();
  config_manager.closeRegistration();

  Configuration::UserValues args{};
  args["python-config-file"].value() = (boost::filesystem::path(getenv("HOME")) / "temp/test_config.py").native();

  config_manager.initialize(args);
  //auto &py = config_manager.getConfiguration<PythonConfig>().getInterpreter();

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
  
  
  SimpleSource source {};
  source.setProperty<WorldCentroid>(5, 10);
  source.setProperty<ShapeParameters>(1,1,1,1,1,1,1,1);
  source.setProperty<IsophotalFlux>(456, 25, 19, 0.3);
  
  ModelFitting::EngineParameterManager mgr {};
  FlexibleModelFittingParameterManager parameter_manager {};
  
  cout << "Parameters:\n";
  for (auto& mfcp : config_manager.getConfiguration<ModelFittingConfig>().getParameters()) {
    auto p = mfcp.second->create(parameter_manager, mgr, source);
    parameter_manager.addParameter(source, mfcp.second, p);
    cout << mfcp.first << ' ' << p->getValue();
    auto free_p = dynamic_pointer_cast<ModelFitting::EngineParameter>(p);
    if (free_p != nullptr) {
      free_p->setEngineValue(-1E10);
      cout << " [" << free_p->getValue();
      free_p->setEngineValue(0);
      cout << ", " << free_p->getValue();
      free_p->setEngineValue(1E10);
      cout << ", " << free_p->getValue() << ']';
    }
    cout << endl;
  }
  
  cout << "\n\nFrames:\n";
  for (auto p : config_manager.getConfiguration<ModelFittingConfig>().getFrames()) {
    cout << p->getFrameNb() << ": number of models " << p->getModels().size() << endl;
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
