/*
 * MultiframeSourceModel.h
 *
 *  Created on: Nov 24, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMESOURCEMODEL_H_
#define _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMESOURCEMODEL_H_

#include "SEFramework/CoordinateSystem/CoordinateSystem.h"
#include "SEFramework/Source/SourceInterface.h"

#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Parameters/EngineParameter.h"
#include "ModelFitting/Parameters/DependentParameter.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/TransformedModel.h"
#include "ModelFitting/Engine/EngineParameterManager.h"

namespace SExtractor {

class MultiframeSourceModel {

  int m_size;
  double m_center_x, m_center_y;
  std::shared_ptr<CoordinateSystem> m_ref_coordinate_system;
  double m_radius_guess, m_aspect_guess, m_exp_flux_guess, m_dev_flux_guess;


  // common parameters
  ModelFitting::EngineParameter dx, dy;

  ModelFitting::ManualParameter exp_xs { 1 };
  ModelFitting::ManualParameter exp_n { 1 };

  ModelFitting::ManualParameter dev_xs { 1 };
  ModelFitting::ManualParameter dev_n { 4 };

  ModelFitting::EngineParameter exp_effective_radius;
  ModelFitting::EngineParameter dev_effective_radius;

  ModelFitting::DependentParameter<ModelFitting::EngineParameter> exp_k;
  ModelFitting::DependentParameter<ModelFitting::EngineParameter> dev_k;

  ModelFitting::EngineParameter exp_aspect, exp_rot;
  ModelFitting::EngineParameter dev_aspect, dev_rot;

  std::vector<std::unique_ptr<ModelFitting::EngineParameter>> exp_fluxes;
  std::vector<std::unique_ptr<ModelFitting::EngineParameter>> dev_fluxes;

  // per band
  std::vector<std::unique_ptr<
      ModelFitting::DependentParameter<
          ModelFitting::EngineParameter, ModelFitting::EngineParameter, ModelFitting::EngineParameter>>> exp_i0s;
  std::vector<std::unique_ptr<
      ModelFitting::DependentParameter<
          ModelFitting::EngineParameter, ModelFitting::EngineParameter, ModelFitting::EngineParameter>>> dev_i0s;

  // per frame
  std::vector<std::unique_ptr<
      ModelFitting::DependentParameter<ModelFitting::EngineParameter, ModelFitting::EngineParameter>>> pixel_x;
  std::vector<std::unique_ptr<
      ModelFitting::DependentParameter<ModelFitting::EngineParameter, ModelFitting::EngineParameter>>> pixel_y;

  std::map<int, int> m_frame_map;
  std::map<int, int> m_frame_band_map;

  int m_number_of_parameters;


  //
//  // per measurement frame parameters
//


public:

  MultiframeSourceModel(const SourceInterface& source);

  void createPlaceholderForInactiveBand();
  void createParamsForBand();

  void createParamsForFrame(int band_nb, int frame_nb, std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset);
  void addModelsForFrame(int frame_nb, std::vector<ModelFitting::TransformedModel>& extended_models, std::tuple<double, double, double, double> jacobian);

  void registerParameters(ModelFitting::EngineParameterManager& manager);

  void debugPrint() const;

  int getSize(const SourceInterface& source) const;

  double getFluxGuess(const SourceInterface& source) const;

  double getRadiusGuess(const SourceInterface& source) const;
  double getAspectGuess(const SourceInterface& source) const;
  double getRotGuess(const SourceInterface& source) const;

  double getCenterX(const SourceInterface& source) const;
  double getCenterY(const SourceInterface& source) const;

  std::shared_ptr<CoordinateSystem> getRefCoordinateSystem(const SourceInterface& source) const;

  int getNumberOfParameters() const;
  WorldCoordinate getFittedWorldCoordinate() const;

  ImageCoordinate getFittedCoordinate() const;

  double getExpFluxForBand(int band_nb) const;
  double getDevFluxForBand(int band_nb) const;

  std::vector<double> getFluxes() const;
  std::vector<double> getExpFluxes() const;
  std::vector<double> getDevFluxes() const;

  SeFloat getExpRadius() const;
  SeFloat getDevRadius() const;

};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MULTIFRAMEMODELFITTING_MULTIFRAMESOURCEMODEL_H_ */
