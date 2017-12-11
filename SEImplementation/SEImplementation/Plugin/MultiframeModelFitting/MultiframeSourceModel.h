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
#include "ModelFitting/Engine/EngineParameterManager.h"

namespace SExtractor {

class MultiframeSourceModel {

  double m_center_x, m_center_y;
  double m_radius_guess, m_aspect_guess, m_exp_flux_guess, m_dev_flux_guess;

  std::shared_ptr<CoordinateSystem> m_ref_coordinate_system;

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

  std::vector<std::unique_ptr<ModelFitting::EngineParameter>> exp_i0s;
  std::vector<std::unique_ptr<ModelFitting::EngineParameter>> dev_i0s;

  std::vector<std::unique_ptr<
      ModelFitting::DependentParameter<ModelFitting::EngineParameter, ModelFitting::EngineParameter>>> pixel_x;
  std::vector<std::unique_ptr<
      ModelFitting::DependentParameter<ModelFitting::EngineParameter, ModelFitting::EngineParameter>>> pixel_y;

  std::vector<int> m_band_nb;

  int m_number_of_parameters;


  //
//  // per measurement frame parameters
//


public:

  MultiframeSourceModel(const SourceInterface& source);

  void createParamsForFrame(std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset, bool first_frame_of_band);
  void addModelsForFrame(int frame_nb, std::vector<ModelFitting::ExtendedModel>& extended_models, int size);
  void registerParameters(ModelFitting::EngineParameterManager& manager);

  void debugPrint() const;

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
