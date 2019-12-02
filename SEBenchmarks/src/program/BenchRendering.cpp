/*
 * BenchRendering.cpp
 *
 *  Created on: Aug 7, 2019
 *      Author: mschefer
 */


#include <boost/timer/timer.hpp>

#include "ElementsKernel/ProgramHeaders.h"

#include "ModelFitting/Models/PointModel.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/TransformedModel.h"
#include "ModelFitting/Models/FrameModel.h"

#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Models/OnlySmooth.h"
#include "ModelFitting/Models/OldSharp.h"
#include "ModelFitting/Models/AutoSharp.h"
#include "ModelFitting/Models/CircularlySymmetricModelComponent.h"
#include "ModelFitting/Models/ScaledModelComponent.h"
#include "ModelFitting/Models/RotatedModelComponent.h"
#include "ModelFitting/Models/PointModel.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/TransformedModel.h"
#include "ModelFitting/Models/CompactSersicModel.h"

#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Image/WriteableImageInterfaceTraits.h"
#include "SEImplementation/Image/ImagePsf.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace ModelFitting;
using namespace SourceXtractor;

template <typename ImageType>
class DummyPsf {
public:
  DummyPsf() : m_kernel(VectorImage<SeFloat>::create(1, 1)) {}

  double getPixelScale() const {
    return 1.0;
  }

  std::size_t getSize() const {
    return 1;
  }

  std::shared_ptr<VectorImage<SourceXtractor::SeFloat>>  getScaledKernel(double /*scale*/) const {
    return m_kernel;
  }

  void convolve(ImageType& /*image*/) const {
  }

private:
  std::shared_ptr<VectorImage<SourceXtractor::SeFloat>>  m_kernel;

};


template <typename ImageType>
class DummyModel : public ExtendedModel<ImageType> {

public:
  DummyModel(BasicParameter& x_scale, BasicParameter& y_scale,
             BasicParameter& rotation, double width, double height,
             BasicParameter& x, BasicParameter& y)
  : ExtendedModel<ImageType>({}, x_scale, y_scale, rotation, width, height, x, y)
    {}

  virtual ~DummyModel() = default;

  double getValue(double x, double y) const override { return 0.0; }

  ImageType getRasterizedImage(double pixel_scale, std::size_t size_x, std::size_t size_y) const override {
    using Traits = ImageTraits<ImageType>;
    ImageType image = Traits::factory(size_x, size_y);
    return image;
  }
};

template <typename ImageType>
class DummyFillModel : public ExtendedModel<ImageType> {

public:
  DummyFillModel(BasicParameter& x_scale, BasicParameter& y_scale,
             BasicParameter& rotation, double width, double height,
             BasicParameter& x, BasicParameter& y)
  : ExtendedModel<ImageType>({}, x_scale, y_scale, rotation, width, height, x, y)
    {}

  virtual ~DummyFillModel() = default;

  double getValue(double x, double y) const override { return 0.0; }

  ImageType getRasterizedImage(double pixel_scale, std::size_t size_x, std::size_t size_y) const override {
    using Traits = ImageTraits<ImageType>;
    ImageType image = Traits::factory(size_x, size_y);

    for (std::size_t x=0; x<size_x; ++x) {
      for (std::size_t y=0; y<size_y; ++y) {
          Traits::at(image, x, y) = x+y;
      }
    }


    return image;
  }
};

template <typename ImageType>
class DummyExpModel : public ExtendedModel<ImageType> {

public:
  DummyExpModel(BasicParameter& x_scale, BasicParameter& y_scale,
             BasicParameter& rotation, double width, double height,
             BasicParameter& x, BasicParameter& y)
  : ExtendedModel<ImageType>({}, x_scale, y_scale, rotation, width, height, x, y)
    {}

  virtual ~DummyExpModel() = default;

  double getValue(double x, double y) const override { return 0.0; }

  ImageType getRasterizedImage(double pixel_scale, std::size_t size_x, std::size_t size_y) const override {
    using Traits = ImageTraits<ImageType>;
    ImageType image = Traits::factory(size_x, size_y);

    for (std::size_t x=0; x<size_x; ++x) {
      for (std::size_t y=0; y<size_y; ++y) {
          Traits::at(image, x, y) = std::exp(float(x+y));
      }
    }

    return image;
  }
};

template <typename ImageType>
class DummySersicModel : public ExtendedModel<ImageType> {

public:
  DummySersicModel(BasicParameter& x_scale, BasicParameter& y_scale,
             BasicParameter& rotation, double width, double height,
             BasicParameter& x, BasicParameter& y)
  : ExtendedModel<ImageType>({}, x_scale, y_scale, rotation, width, height, x, y)
    {}

  virtual ~DummySersicModel() = default;

  double getValue(double x, double y) const override { return 0.0; }

  ImageType getRasterizedImage(double pixel_scale, std::size_t size_x, std::size_t size_y) const override {
    using Traits = ImageTraits<ImageType>;
    ImageType image = Traits::factory(size_x, size_y);

    for (std::size_t x=0; x<size_x; ++x) {
      for (std::size_t y=0; y<size_y; ++y) {
          Traits::at(image, x, y) = 2.0f * std::exp(-1.5f * std::pow(float(x*x+y*y), 1.f / 4.0f));;
      }
    }

    return image;
  }
};




class BenchRendering : public Elements::Program {

public:

//  po::options_description defineSpecificProgramOptions() override {
//    po::options_description config_options { "TestImage options" };
//
//    // Add the specific program options
//    config_options.add_options()
//        ("output", po::value<string>()->required(), "filename to save the created test image")
//        ;
//
//    return config_options;
//  }



  FrameModel<DummyPsf<ImageInterfaceTypePtr>, ImageInterfaceTypePtr> makeEmptyFrameModel() {
    std::vector<ConstantModel> constant_models;
    std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>> extended_models;
    std::vector<PointModel> point_models;

    double pixel_scale = 1.0f;
    int image_size = 256;

    FrameModel<DummyPsf<ImageInterfaceTypePtr>, ImageInterfaceTypePtr> frame_model {
      pixel_scale,
      (std::size_t) image_size, (std::size_t) image_size,
      std::move(constant_models),
      std::move(point_models),
      std::move(extended_models),
    };

    return frame_model;
  }

  FrameModel<DummyPsf<ImageInterfaceTypePtr>, ImageInterfaceTypePtr> makeSersicFrameModel() {
    std::vector<ConstantModel> constant_models;
    std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>> extended_models;
    std::vector<PointModel> point_models;

    // Devaucouleurs component
    ManualParameter x_param {128};
    ManualParameter y_param {128};

    ManualParameter xs (1);
    ManualParameter ys (1);
    ManualParameter rot (0);
    ManualParameter dev_n { 4 };
    ManualParameter dev_k { 10. };
    ManualParameter dev_i0 { 10000 };

    std::vector<std::unique_ptr<ModelComponent>> component_list {};
    auto exp = make_unique<SersicModelComponent>(make_unique<OldSharp>(), dev_i0, dev_n, dev_k);
    component_list.clear();
    component_list.emplace_back(std::move(exp));
    extended_models.emplace_back(std::make_shared<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>(
        std::move(component_list), xs, ys, rot, 256, 256, x_param, y_param));


    double pixel_scale = 1.0f;
    int image_size = 256;

    FrameModel<DummyPsf<ImageInterfaceTypePtr>, ImageInterfaceTypePtr> frame_model {
      pixel_scale,
      (std::size_t) image_size, (std::size_t) image_size,
      std::move(constant_models),
      std::move(point_models),
      std::move(extended_models),
    };

    return frame_model;
  }

  FrameModel<DummyPsf<ImageInterfaceTypePtr>, ImageInterfaceTypePtr> makeCompactSersicFrameModel() {
    std::vector<ConstantModel> constant_models;
    std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>> extended_models;
    std::vector<PointModel> point_models;

    // Devaucouleurs component
    ManualParameter x_param {128};
    ManualParameter y_param {128};

    ManualParameter xs (1);
    ManualParameter ys (1);
    ManualParameter rot (0);
    ManualParameter n { 4 };
    ManualParameter k { 10. };
    ManualParameter i0 { 10000 };

    extended_models.emplace_back(std::make_shared<ModelFitting::CompactSersicModel<ImageInterfaceTypePtr>>(
        3.0, i0, k, n,
        xs, ys, rot, 256, 256, x_param, y_param, std::make_tuple(1, 0, 0, 1)));

    double pixel_scale = 1.0f;
    int image_size = 256;

    FrameModel<DummyPsf<ImageInterfaceTypePtr>, ImageInterfaceTypePtr> frame_model {
      pixel_scale,
      (std::size_t) image_size, (std::size_t) image_size,
      std::move(constant_models),
      std::move(point_models),
      std::move(extended_models),
    };

    return frame_model;
  }

  template<typename T>
  FrameModel<DummyPsf<ImageInterfaceTypePtr>, ImageInterfaceTypePtr> makeDummyFrameModel() {
    std::vector<ConstantModel> constant_models;
    std::vector<std::shared_ptr<ModelFitting::ExtendedModel<ImageInterfaceTypePtr>>> extended_models;
    std::vector<PointModel> point_models;

    // Devaucouleurs component
    ManualParameter x_param {128};
    ManualParameter y_param {128};

    ManualParameter xs (1);
    ManualParameter ys (1);
    ManualParameter rot (0);
    ManualParameter n { 4 };
    ManualParameter k { 10. };
    ManualParameter i0 { 10000 };

    extended_models.emplace_back(std::make_shared<T>(
        xs, ys, rot, 256, 256, x_param, y_param));

    double pixel_scale = 1.0f;
    int image_size = 256;

    FrameModel<DummyPsf<ImageInterfaceTypePtr>, ImageInterfaceTypePtr> frame_model {
      pixel_scale,
      (std::size_t) image_size, (std::size_t) image_size,
      std::move(constant_models),
      std::move(point_models),
      std::move(extended_models),
    };

    return frame_model;
  }


  template<typename T>
  std::shared_ptr<VectorImage<SeFloat>> measureRasterToImage(int iterations, T& frame_model) {
    auto image = VectorImage<SeFloat>::create(256, 256);

    for (int i=0; i<iterations; i++) {
      frame_model.rasterToImage(image);
    }

    return image;
  }

  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {
    Elements::Logging logger = Elements::Logging::getLogger("BenchRendering");

    int iterations = 300;

    auto empty_frame_model = makeEmptyFrameModel();
    auto dummy_frame_model = makeDummyFrameModel<DummyModel<ImageInterfaceTypePtr>>();
    auto dummy_fill_frame_model = makeDummyFrameModel<DummyFillModel<ImageInterfaceTypePtr>>();
    auto dummy_exp_frame_model = makeDummyFrameModel<DummyExpModel<ImageInterfaceTypePtr>>();
    auto dummy_sersic_frame_model = makeDummyFrameModel<DummySersicModel<ImageInterfaceTypePtr>>();
    auto sersic_frame_model = makeSersicFrameModel();
    auto compact_frame_model = makeCompactSersicFrameModel();

    logger.info() << "Testing with empty frame";
    {
      boost::timer::auto_cpu_timer t;
      measureRasterToImage(iterations, empty_frame_model);
    }
    logger.info() << "Testing with dummy (nop) model";
    {
      boost::timer::auto_cpu_timer t;
      measureRasterToImage(iterations, dummy_frame_model);
    }
    logger.info() << "Testing with dummy (fill) model";
    {
      boost::timer::auto_cpu_timer t;
      measureRasterToImage(iterations, dummy_fill_frame_model);
    }
    logger.info() << "Testing with dummy (fill exp) model";
    {
      boost::timer::auto_cpu_timer t;
      measureRasterToImage(iterations, dummy_exp_frame_model);
    }
    logger.info() << "Testing with dummy (sersic) model";
    {
      boost::timer::auto_cpu_timer t;
      measureRasterToImage(iterations, dummy_sersic_frame_model);
    }
    logger.info() << "Testing with old style Sersic model";
    {
      boost::timer::auto_cpu_timer t;
      measureRasterToImage(iterations, sersic_frame_model);
    }
    logger.info() << "Testing with compact Sersic model";
    {
      boost::timer::auto_cpu_timer t;
      measureRasterToImage(iterations, compact_frame_model);
    }

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(BenchRendering)
