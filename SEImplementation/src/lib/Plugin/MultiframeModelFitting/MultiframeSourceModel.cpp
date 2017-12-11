/*
 * MultiframeSourceModel.cpp
 *
 *  Created on: Nov 24, 2017
 *      Author: mschefer
 */

#include "ModelFitting/utils.h"

#include "ModelFitting/Parameters/ExpSigmoidConverter.h"
#include "ModelFitting/Parameters/SigmoidConverter.h"
#include "ModelFitting/Parameters/NormalizedConverter.h"

#include "ModelFitting/Models/OnlySmooth.h"
#include "ModelFitting/Models/OldSharp.h"
#include "ModelFitting/Models/AutoSharp.h"

#include "ModelFitting/Models/CircularlySymmetricModelComponent.h"

#include "SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"

#include "SEImplementation/Plugin/MultiframeModelFitting/MultiframeSourceModel.h"

namespace SExtractor {

using namespace ModelFitting;

MultiframeSourceModel::MultiframeSourceModel(const SourceInterface& source) :
    m_center_x(getCenterX(source)),
    m_center_y(getCenterY(source)),
    m_ref_coordinate_system(getRefCoordinateSystem(source)),

    m_radius_guess(getRadiusGuess(source)),
    m_aspect_guess(getAspectGuess(source)),
    m_exp_flux_guess(getFluxGuess(source)),
    m_dev_flux_guess(getFluxGuess(source)),

    dx(0, make_unique<SigmoidConverter>(-m_radius_guess, m_radius_guess)),
    dy(0, make_unique<SigmoidConverter>(-m_radius_guess, m_radius_guess)),

    exp_effective_radius(m_radius_guess, make_unique<ExpSigmoidConverter>(m_radius_guess * 0.001, m_radius_guess * 100)),
    dev_effective_radius(m_radius_guess, make_unique<ExpSigmoidConverter>(m_radius_guess * 0.001, m_radius_guess * 100)),

    exp_k(
        [](double eff_radius) { return 1.7 / eff_radius; },
        exp_effective_radius),
    dev_k(
        [](double eff_radius) { return pow(3459.0 / eff_radius, .25); },
        dev_effective_radius),

    exp_aspect(m_aspect_guess, make_unique<SigmoidConverter>(0, 1.01)),
    exp_rot(getRotGuess(source), make_unique<SigmoidConverter>(-M_PI, M_PI)),
    dev_aspect(m_aspect_guess, make_unique<SigmoidConverter>(0, 1.01)),
    dev_rot(getRotGuess(source), make_unique<SigmoidConverter>(-M_PI, M_PI)),

    m_number_of_parameters(0)
  {
  }

  void MultiframeSourceModel::createParamsForFrame(std::shared_ptr<CoordinateSystem> coordinates, PixelCoordinate offset, bool first_frame_of_band) {
    if (first_frame_of_band) {
      if (m_band_nb.size()==0) {
        m_band_nb.emplace_back(0);
      } else {
        m_band_nb.emplace_back(m_band_nb.back()+1);
      }
      auto exp_i0_guess = m_exp_flux_guess / (M_PI * 2.0 * 0.346 * m_radius_guess * m_radius_guess * m_aspect_guess);
      exp_i0s.emplace_back(new EngineParameter(exp_i0_guess, make_unique<ExpSigmoidConverter>(exp_i0_guess * .00001, exp_i0_guess * 20)));

      auto dev_i0_guess = m_dev_flux_guess * pow(10, 3.33) / (7.2 * M_PI * m_radius_guess * m_radius_guess * m_aspect_guess);
      dev_i0s.emplace_back(new EngineParameter(dev_i0_guess, make_unique<ExpSigmoidConverter>(dev_i0_guess * .00001, dev_i0_guess * 20)));
    } else {
      m_band_nb.emplace_back(m_band_nb.back());
    }

    auto cx = m_center_x;
    auto cy = m_center_y;
    auto cs = m_ref_coordinate_system;

    pixel_x.emplace_back(new DependentParameter<EngineParameter, EngineParameter>(
        [coordinates, cx, cy, offset, cs](double dx, double dy) {
          return coordinates->worldToImage(cs->imageToWorld(ImageCoordinate(cx + dx, cy + dy))).m_x - offset.m_x;
        }, dx, dy));
    pixel_y.emplace_back(new DependentParameter<EngineParameter, EngineParameter>(
        [coordinates, cx, cy, offset, cs](double dx, double dy) {
          return coordinates->worldToImage(cs->imageToWorld(ImageCoordinate(cx + dx, cy + dy))).m_y - offset.m_y;
        }, dx, dy));
  }

  void MultiframeSourceModel::addModelsForFrame(int frame_nb, std::vector<ExtendedModel>& extended_models, int size) {
    // exponential
    {
      std::vector<std::unique_ptr<ModelComponent>> component_list {};
      auto exp = make_unique<SersicModelComponent>(make_unique<OnlySmooth>(), *exp_i0s[m_band_nb[frame_nb]], exp_n, exp_k);
      //auto exp = make_unique<SersicModelComponent>(make_unique<OnlySmooth>(), exp_i0, exp_n, exp_k);
      component_list.clear();
      component_list.emplace_back(std::move(exp));
      extended_models.emplace_back(std::move(component_list), exp_xs, exp_aspect, exp_rot, size, size, *(pixel_x[frame_nb]), *(pixel_y[frame_nb]));
    }
    // devaucouleurs
    {
      std::vector<std::unique_ptr<ModelComponent>> component_list {};
      auto dev = make_unique<SersicModelComponent>(make_unique<OldSharp>(), *dev_i0s[m_band_nb[frame_nb]], dev_n, dev_k);
      //auto dev = make_unique<SersicModelComponent>(make_unique<AutoSharp>(), dev_i0, dev_n, dev_k);
      component_list.clear();
      component_list.emplace_back(std::move(dev));
      extended_models.emplace_back(std::move(component_list), dev_xs, dev_aspect, dev_rot, size, size, *(pixel_x[frame_nb]), *(pixel_y[frame_nb]));
    }
  }

  void MultiframeSourceModel::registerParameters(EngineParameterManager& manager) {
    manager.registerParameter(dx);
    manager.registerParameter(dy);
    manager.registerParameter(exp_effective_radius);
    manager.registerParameter(dev_effective_radius);

    manager.registerParameter(exp_aspect);
    manager.registerParameter(dev_aspect);
    manager.registerParameter(exp_rot);
    manager.registerParameter(dev_rot);

    m_number_of_parameters += 8;

//    manager.registerParameter(exp_i0);
//    manager.registerParameter(dev_i0);

    for (auto& exp_i0 : exp_i0s) {
      manager.registerParameter(*exp_i0);
      m_number_of_parameters++;
    }

    for (auto& dev_i0 : dev_i0s) {
      manager.registerParameter(*dev_i0);
      m_number_of_parameters++;
    }
  }

  void MultiframeSourceModel::debugPrint() const {
    //std::cout << "\tsize: " << m_size << "\n";
    std::cout << "\tx: " << dx.getValue() << "\ty: " << dy.getValue() << "\n";
    for (auto& exp_i0 : exp_i0s) {
      std::cout << exp_i0->getValue() << " ";
    }
    std::cout << "\n";
    //std::cout << "\texp i0: " << exp_i0.getValue()<< "\tReff: " << exp_effective_radius.getValue() << "\n";
    //std::cout << "\tdev i0: " << dev_i0.getValue()<< "\tReff: " << dev_effective_radius.getValue() << "\n";
  }

  double MultiframeSourceModel::getFluxGuess(const SourceInterface& source) const {
    auto iso_flux = source.getProperty<IsophotalFlux>().getFlux();
    return iso_flux / 2;
  }

  double MultiframeSourceModel::getRadiusGuess(const SourceInterface& source) const {
    auto& shape_parameters = source.getProperty<ShapeParameters>();
    double radius_guess = shape_parameters.getEllipseA() / 2.0;

    return radius_guess;
  }

  double MultiframeSourceModel::getAspectGuess(const SourceInterface& source) const {
    auto& shape_parameters = source.getProperty<ShapeParameters>();
    double aspect_guess = std::max<double>(shape_parameters.getEllipseB() / shape_parameters.getEllipseA(), 0.01);

    return aspect_guess;
  }

  double MultiframeSourceModel::getRotGuess(const SourceInterface& source) const {
    auto& shape_parameters = source.getProperty<ShapeParameters>();
    double rot_guess = shape_parameters.getEllipseTheta();

    return -rot_guess;
  }

  double MultiframeSourceModel::getCenterX(const SourceInterface& source) const {
    auto& centroid = source.getProperty<PixelCentroid>();
    return centroid.getCentroidX();
  }

  double MultiframeSourceModel::getCenterY(const SourceInterface& source) const {
    auto& centroid = source.getProperty<PixelCentroid>();
    return centroid.getCentroidY();
  }

  std::shared_ptr<CoordinateSystem> MultiframeSourceModel::getRefCoordinateSystem(const SourceInterface& source) const {
    auto frame = source.getProperty<DetectionFrame>().getFrame();
    return frame->getCoordinateSystem();
  }

  int MultiframeSourceModel::getNumberOfParameters() const {
    return m_number_of_parameters;
  }

  WorldCoordinate MultiframeSourceModel::getFittedWorldCoordinate() const {
    return m_ref_coordinate_system->imageToWorld(ImageCoordinate(m_center_x + dx.getValue(), m_center_y + dy.getValue()));
  }

  ImageCoordinate MultiframeSourceModel::getFittedCoordinate() const {
    return ImageCoordinate(m_center_x + dx.getValue(), m_center_y + dy.getValue());
  }

  double MultiframeSourceModel::getExpFluxForBand(int band_nb) const {
    double total_flux = 0;

    auto exp_i0 = exp_i0s[band_nb]->getValue();
    auto exp_radius = exp_effective_radius.getValue();
    total_flux += exp_i0 * (M_PI * 2.0 * 0.346 * exp_radius * exp_radius * exp_aspect.getValue());

    return total_flux;
  }

  double MultiframeSourceModel::getDevFluxForBand(int band_nb) const {
    double total_flux = 0;

    auto dev_i0 = dev_i0s[band_nb]->getValue();
    auto dev_radius = dev_effective_radius.getValue();
    total_flux += dev_i0 * (7.2 * M_PI * dev_radius * dev_radius * dev_aspect.getValue()) / pow(10, 3.33);

    return total_flux;
  }

  std::vector<double> MultiframeSourceModel::getFluxes() const {
    std::vector<double> fluxes;

    for (unsigned int i=0; i<exp_i0s.size(); i++) {
      fluxes.push_back(getExpFluxForBand(i)+getDevFluxForBand(i));
    }

    return fluxes;
  }

  std::vector<double> MultiframeSourceModel::getExpFluxes() const {
    std::vector<double> fluxes;

    for (unsigned int i=0; i<exp_i0s.size(); i++) {
      fluxes.push_back(getExpFluxForBand(i));
    }

    return fluxes;
  }

  std::vector<double> MultiframeSourceModel::getDevFluxes() const {
    std::vector<double> fluxes;

    for (unsigned int i=0; i<exp_i0s.size(); i++) {
      fluxes.push_back(getDevFluxForBand(i));
    }

    return fluxes;
  }

  SeFloat MultiframeSourceModel::getExpRadius() const {
    return exp_effective_radius.getValue();
  }

  SeFloat MultiframeSourceModel::getDevRadius() const {
    return dev_effective_radius.getValue();
  }
}

