/*
 * SimpleModelFitting_test.cpp
 *
 *  Created on: Jun 30, 2017
 *      Author: mschefer
 */

#include <boost/test/unit_test.hpp>

#include <memory>


#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Source/SimpleSourceGroup.h"

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/ConvolutionImageProcessing.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h"
#include "SEImplementation/Plugin/Psf/PsfProperty.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h"
#include "SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Plugin/Psf/PsfPluginConfig.h"
#include "SEImplementation/Plugin/SimpleModelFitting/SimpleModelFitting.h"
#include "SEImplementation/Plugin/SimpleModelFitting/SimpleModelFittingTask.h"

using namespace SExtractor;

class DummyCoordinateSystem : public CoordinateSystem {
public:

  virtual WorldCoordinate imageToWorld(ImageCoordinate ) const override {
    return WorldCoordinate(0,0);
  }

  virtual ImageCoordinate worldToImage(WorldCoordinate ) const override {
    return ImageCoordinate(0,0);
  }
};


//std::shared_ptr<VectorImage<SeFloat>> convertPsf(const cv::Mat& mat) {
//  auto image = VectorImage<SeFloat>::create(mat.rows, mat.cols);
//
//  for (int y=0; y < image->getHeight(); y++) {
//    for (int x=0; x < image->getWidth(); x++) {
//       image->setValue(x, y, mat.at<double>(y, x));
//    }
//  }
//
//  return image;
//}

struct SimpleModelFittingFixture {
  std::shared_ptr<SimpleSource> source {new SimpleSource};
  SimpleSourceGroup group;

  std::shared_ptr<ImagePsf> psf;
  std::shared_ptr<SimpleModelFittingTask> model_fitting_task;

  SimpleModelFittingFixture() {
    group.addSource(source);
    psf = std::make_shared<ImagePsf>(1, PsfPluginConfig::generateGaussianPsf(5, 1)->getPsf({}));
    model_fitting_task = std::make_shared<SimpleModelFittingTask>(100);
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SimpleModelFitting_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(modelfitting_test, SimpleModelFittingFixture) {
  auto image = psf->getScaledKernel(10);
  std::vector<PixelCoordinate> pixel_coordinates;
  for (auto x = 0; x < image->getWidth(); ++x) {
    for (auto y = 0; y < image->getHeight(); ++y) {
      pixel_coordinates.emplace_back(x, y);
    }
  }

  auto variance_image = VectorImage<SeFloat>::create(image->getWidth(), image->getHeight());
  variance_image->fillValue(0);

  auto detection_frame = std::make_shared<DetectionImageFrame>(
    image, nullptr, 10,
    std::make_shared<DummyCoordinateSystem>(), 1, 65000, 1);

  source->setProperty<ShapeParameters>(5, 5, 0, 1, 1, 1, 1, 25);
  source->setProperty<PixelCentroid>(image->getWidth()/2 - 1, image->getHeight() / 2 - 2);
  source->setProperty<PixelBoundaries>(0 , 0, image->getWidth(), image->getHeight());
  source->setProperty<IsophotalFlux>(10., 0., 1., 0.);
  source->setProperty<PixelCoordinateList>(pixel_coordinates);
  source->setProperty<DetectionFrame>(detection_frame);

  group.setProperty<DetectionFrameGroupStamp>(image, image, PixelCoordinate(0,0), variance_image);
  group.setProperty<PsfProperty>(std::move(*psf));
  model_fitting_task->computeProperties(group);

  BOOST_CHECK(true);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


