/*
 * SimpleModelTask.cpp
 *
 *  Created on: May 2, 2017
 *      Author: mschefer
 */

#include <iostream>
#include <tuple>
#include <vector>
#include <valarray>
#include <boost/any.hpp>

#include <CCfits/CCfits>

#include "ElementsKernel/PathSearch.h"

#include "SEImplementation/Image/ImageInterfaceTraits.h"

#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Parameters/DependentParameter.h"

#include "ModelFitting/Parameters/ExpSigmoidConverter.h"
#include "ModelFitting/Parameters/SigmoidConverter.h"
#include "ModelFitting/Parameters/NormalizedConverter.h"
#include "ModelFitting/Models/OnlySmooth.h"
#include "ModelFitting/Models/AutoSharp.h"
#include "ModelFitting/Models/CircularlySymmetricModelComponent.h"
#include "ModelFitting/Models/ScaledModelComponent.h"
#include "ModelFitting/Models/RotatedModelComponent.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Image/OpenCvMatImageTraits.h"
#include "ModelFitting/utils.h"
#include "ModelFitting/Models/FrameModel.h"
#include "ModelFitting/Engine/ResidualEstimator.h"
#include "ModelFitting/Engine/LevmarEngine.h"
#include "ModelFitting/Image/OpenCvPsf.h"

#include "ModelFitting/Engine/OpenCvDataVsModelInputTraits.h"

#include "ModelFitting/Engine/LogChiSquareComparator.h"


#include "ModelFitting/Engine/EngineParameterManager.h"
#include "ModelFitting/Engine/DataVsModelResiduals.h"

#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h"

#include "SEImplementation/Plugin/SimpleModelFitting/SimpleModelFitting.h"
#include "SEImplementation/Plugin/SimpleModelFitting/SimpleModelFittingTask.h"

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"

#include "SEFramework/Property/DetectionFrame.h"


namespace SExtractor {

using namespace ModelFitting;


/////////// FIXME copypasta from ModelFitting tests, move somewhere else

template<typename T>
void writeToFits(const T& image, const std::string& filename) {
  auto total = ModelFitting::ImageTraits<T>::height(image) * ModelFitting::ImageTraits<T>::width(image);
  std::valarray<double> data (total);
  std::copy(ModelFitting::ImageTraits<T>::begin(image), ModelFitting::ImageTraits<T>::end(image), begin(data));
  long naxis = 2;
  long naxes[2] = { ModelFitting::ImageTraits<T>::width(image), ModelFitting::ImageTraits<T>::height(image) };
  std::unique_ptr<CCfits::FITS> pFits {new CCfits::FITS("!"+filename, DOUBLE_IMG, naxis, naxes)};
  pFits->pHDU().write(1, total, data);
  std::cout << "Created file " << filename << '\n';
}

/// Reads a PSF from a fits file. The image must be square and have sides of odd
/// number of pixels. The pixel scale is read by the header keyword SCALE which
/// must be present
ModelFitting::OpenCvPsf readPsf(const std::string& filename) {
  // Read the HDU from the file
  std::unique_ptr<CCfits::FITS> pFits {new CCfits::FITS(filename)};
  auto& image_hdu = pFits->pHDU();
  // Get the pixel scale from the header
  double pixel_scale = 0.;
  image_hdu.readKey("SCALE", pixel_scale);
  // Get the dimension of the image
  if (image_hdu.axis(0) != image_hdu.axis(1)) {
    throw Elements::Exception() << "Non square PSF (" << image_hdu.axis(0) << 'X'
                                << image_hdu.axis(1) << ')';
  }
  auto size = image_hdu.axis(0);
  // Get the data
  std::valarray<double> data {};
  image_hdu.read(data);
  cv::Mat kernel (size, size, CV_64F);
  std::copy(begin(data), end(data), kernel.begin<double>());
  return ModelFitting::OpenCvPsf {pixel_scale, kernel};
}

// Prints on the screen the info of the levmar minimization
void printLevmarInfo(std::array<double,10> info) {
  std::cout << "\nMinimization info:\n";
  std::cout << "  ||e||_2 at initial p: " << info[0] << '\n';
  std::cout << "  ||e||_2: " << info[1] << '\n';
  std::cout << "  ||J^T e||_inf: " << info[2] << '\n';
  std::cout << "  ||Dp||_2: " << info[3] << '\n';
  std::cout << "  mu/max[J^T J]_ii: " << info[4] << '\n';
  std::cout << "  # iterations: " << info[5] << '\n';
  switch ((int)info[6]) {
  case 1:
    std::cout << "  stopped by small gradient J^T e\n";
    break;
  case 2:
    std::cout << "  stopped by small Dp\n";
    break;
  case 3:
    std::cout << "  stopped by itmax\n";
    break;
  case 4:
    std::cout << "  singular matrix. Restart from current p with increased mu\n";
    break;
  case 5:
    std::cout << "  no further error reduction is possible. Restart with increased mu\n";
    break;
  case 6:
    std::cout << "  stopped by small ||e||_2\n";
    break;
  case 7:
    std::cout << "  stopped by invalid (i.e. NaN or Inf) func values; a user error\n";
    break;
  }
  std::cout << "  # function evaluations: " << info[7] << '\n';
  std::cout << "  # Jacobian evaluations: " << info[8] << '\n';
  std::cout << "  # linear systems solved: " << info[9] << "\n\n";
}

///////////////////////////////////////////////////////////////////////////////

struct SourceModel {
  double size;
  EngineParameter x, y, x_scale, y_scale, rot_angle, i0, n, k;

  void registerParameters(EngineParameterManager& manager) {
    manager.registerParameter(x);
    manager.registerParameter(y);
    manager.registerParameter(x_scale);
    manager.registerParameter(y_scale);
    manager.registerParameter(rot_angle);

    manager.registerParameter(i0);
    manager.registerParameter(k);
    manager.registerParameter(n);
  }

  void debugPrint() const {
    std::cout << "\tx: " << x.getValue() << "\ty: " << y.getValue() << "\ti0: " << i0.getValue()<< "\tk: " << k.getValue() << "\tn: " << n.getValue() << "\n";
    std::cout << "\txs: " << x_scale.getValue() << "\tys: " << y_scale.getValue() << "\trot: " << rot_angle.getValue() << "\n";
  }
};

void SimpleModelFittingTask::computeProperties(SourceGroupInterface& group) const {
  auto& group_stamp = group.getProperty<DetectionFrameGroupStamp>().getStamp();
  PixelCoordinate stamp_top_left = group.getProperty<DetectionFrameGroupStamp>().getTopLeft();


  if (m_check_image == nullptr) {
    auto image = group.cbegin()->getProperty<DetectionFrame>().getFrame()->getOriginalImage();
    const_cast<SimpleModelFittingTask*>(this)->m_check_image = std::make_shared<VectorImage<DetectionImage::PixelType>>(image->getWidth(), image->getHeight());
  }

  double pixel_scale = 1;

  EngineParameterManager manager {};
  std::vector<ConstantModel> constant_models {};
  std::vector<ExtendedModel> extended_models {};

  std::vector<SourceModel> source_models;

  for (auto& source : group) {
    auto& pixel_centroid = source.getProperty<PixelCentroid>();
    auto& shape_parameters = source.getProperty<ShapeParameters>();
    auto& pixel_boundaries = source.getProperty<PixelBoundaries>();
    auto& peak_value = source.getProperty<PeakValue>();

    double size_factor = 1.5;
    auto size = std::max<double>(pixel_boundaries.getWidth(), pixel_boundaries.getHeight()) * size_factor;

    SourceModel source_model {
      size,
      {pixel_centroid.getCentroidX() - stamp_top_left.m_x, make_unique<NormalizedConverter>(10.)},
      {pixel_centroid.getCentroidY() - stamp_top_left.m_y, make_unique<NormalizedConverter>(10.)},
      {1.0 / size_factor, make_unique<SigmoidConverter>(0, 10.)},
      {1.0 / size_factor, make_unique<SigmoidConverter>(0, 10.)},
      {shape_parameters.getEllipseTheta(), make_unique<SigmoidConverter>(-2*M_PI, 2*M_PI)},
      {peak_value.getMaxValue(), make_unique<ExpSigmoidConverter>(1, 1000000.)},
      {1, make_unique<SigmoidConverter>(0, 100)},
      {1, make_unique<SigmoidConverter>(0, 100)}
    };

    auto reg_man = make_unique<OnlySmooth>();
    auto exp = make_unique<SersicModelComponent>(std::move(reg_man), source_model.i0, source_model.n, source_model.k);
    std::vector<std::unique_ptr<ModelComponent>> component_list {};
    component_list.emplace_back(move(exp));

    extended_models.emplace_back(std::move(component_list), source_model.x_scale, source_model.y_scale,
        source_model.rot_angle, size, size, source_model.x, source_model.y);

    source_models.push_back(source_model);
  }

  for (auto& source_model : source_models) {
    source_model.registerParameters(manager);
  }

//  EngineParameter back {0.01, make_unique<ExpSigmoidConverter>(0.0000001, 1000000.)};
//  constant_models.emplace_back(back);
//  manager.registerParameter(back);

  auto psf_path = Elements::pathSearchInEnvVariable("psf_gal.fits", "ELEMENTS_AUX_PATH");
  auto psf = readPsf(psf_path[0].string());

  FrameModel<OpenCvPsf, cv::Mat> frame_model {
    pixel_scale, (size_t) group_stamp.getWidth(), (size_t) group_stamp.getHeight(), std::move(constant_models), {},
    std::move(extended_models), std::move(psf)
  };

  cv::Mat weight = cv::Mat::ones(group_stamp.getHeight(), group_stamp.getWidth(), CV_64F);
  for (auto& source : group) {
    auto& pixels =  source.getProperty<PixelCoordinateList>().getCoordinateList();
    for (auto pixel : pixels) {
      pixel -= stamp_top_left;
      weight.at<double>(pixel.m_y, pixel.m_x) = 10;
    }
  }

  // FIXME for now just copy to an opencv image
  cv::Mat image (group_stamp.getHeight(), group_stamp.getWidth(), CV_64F);
  for (int y=0; y < group_stamp.getHeight(); y++) {
    for (int x=0; x < group_stamp.getWidth(); x++) {
      image.at<double>(y, x) = group_stamp.getValue(x, y);
    }
  }


  auto data_vs_model = createDataVsModelResiduals(image, std::move(frame_model),
      std::move(weight), LogChiSquareComparator{});

  ResidualEstimator res_estimator {};
  res_estimator.registerBlockProvider(move(data_vs_model));

  LevmarEngine engine {m_max_iterations};

  for (auto& source_model : source_models) {
    std::cout << "Before: ";
    source_model.debugPrint();
  }

  auto solution = engine.solveProblem(manager, res_estimator);

  for (auto& source_model : source_models) {
    std::cout << "After:  ";
    source_model.debugPrint();
  }

  printLevmarInfo(boost::any_cast<std::array<double,10>>(solution.underlying_framework_info));

  size_t iterations = (size_t) boost::any_cast<std::array<double,10>>(solution.underlying_framework_info)[5];

  auto source_model_iter = source_models.begin();
  for (auto& source : group) {
    auto& source_model = *(source_model_iter++);
  }


  auto source_iter = group.begin();
  for (auto& source_model : source_models) {
    auto& source = *source_iter;
    ++source_iter;

    std::vector<ConstantModel> constant_models {};
    std::vector<ExtendedModel> extended_models {};

    auto reg_man = make_unique<OnlySmooth>();
    auto exp = make_unique<SersicModelComponent>(move(reg_man), source_model.i0, source_model.n, source_model.k);
    std::vector<std::unique_ptr<ModelComponent>> component_list {};
    component_list.clear();
    component_list.emplace_back(move(exp));
    extended_models.clear();
    extended_models.emplace_back(move(component_list),source_model.x_scale, source_model.y_scale,
        source_model.rot_angle, source_model.size, source_model.size, source_model.x, source_model.y);
    constant_models.clear();
    //constant_models.emplace_back(back);

    auto psf_path = Elements::pathSearchInEnvVariable("psf_gal.fits", "ELEMENTS_AUX_PATH");
    auto psf = readPsf(psf_path[0].string());

    FrameModel<OpenCvPsf, cv::Mat> frame_model_after {
      1, group_stamp.getWidth(), group_stamp.getHeight(), std::move(constant_models), {},
          std::move(extended_models), std::move(psf)
    };

    auto final_image = frame_model_after.getImage();

    double total_flux = 0;
    for (int y=0; y < group_stamp.getHeight(); y++) {
      for (int x=0; x < group_stamp.getWidth(); x++) {
        PixelCoordinate pixel (x,y);
        pixel += stamp_top_left;
        m_check_image->setValue(pixel, m_check_image->getValue(pixel) + final_image.at<double>(y, x));
        //m_check_image->setValue(pixel, group_stamp.getValue(x,y));

        total_flux += final_image.at<double>(y, x);
      }
    }

    source.setProperty<SimpleModelFitting>(total_flux, source_model.i0.getValue(), iterations);
  }
  writeToFits(m_check_image, "mf_checkimage.fits");

//  auto filtered_image = std::make_shared<VectorImage<DetectionImage::PixelType>> (*(group.cbegin()->getProperty<DetectionFrame>().getFrame()->getThresholdedImage()));
//  for (int y=0; y < filtered_image->getHeight(); y++) {
//    for (int x=0; x < filtered_image->getWidth(); x++) {
//      if (filtered_image->getValue(x,y) < 0) {
//        filtered_image->setValue(x,y,0);
//      }
//    }
//  }
//  writeToFits(filtered_image, "mf_checkimage_filtered.fits");
}


}


