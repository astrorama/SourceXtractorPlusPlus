/*
 * PointModelFitting_test.cpp
 *
 *  Created on: July 11, 2018
 *      Author: Alejandro ��lvarez
 */

#include <boost/test/unit_test.hpp>
#include <SEImplementation/Plugin/PointModelFitting/PointModelFitting.h>
#include "SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h"
#include "SEImplementation/Plugin/Psf/PsfProperty.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEFramework/Property/DetectionFrame.h"
#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Source/SimpleSourceGroup.h"
#include "SEFramework/CoordinateSystem/CoordinateSystem.h"
#include "SEImplementation/Image/ImagePsf.h"
#include "SEImplementation/Plugin/Psf/PsfPluginConfig.h"
#include "SEImplementation/Plugin/PointModelFitting/PointModelFittingTask.h"


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


struct PointModelFittingFixture {
  std::shared_ptr<ImagePsf> psf;
  std::shared_ptr<SimpleSource> source {new SimpleSource};
  SimpleSourceGroup group;

  PointModelFittingFixture() {
    psf = std::make_shared<ImagePsf>(1, PsfPluginConfig::generateGaussianPsf(5, 1)->getPsf({}));
    group.addSource(source);
  }
};


BOOST_AUTO_TEST_SUITE(PointModelFitting_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (point_model_fitting_test, PointModelFittingFixture) {
  auto image = psf->getScaledKernel(10);
  std::vector<PixelCoordinate> pixel_coordinates;
  for (auto x = 0; x < image->getWidth(); ++x) {
    for (auto y = 0; y < image->getHeight(); ++y) {
      pixel_coordinates.emplace_back(x, y);
    }
  }

  auto variance_image = VectorImage<SeFloat>::create(image->getWidth(), image->getHeight());
  variance_image->fillValue(0);

  auto detection_frame = std::make_shared<Frame<DetectionImage::PixelType>>(image, nullptr, 10,
    std::make_shared<DummyCoordinateSystem>(), 1, 65000, 1);

  source->setProperty<PixelCentroid>(image->getWidth()/2 - 1, image->getHeight() / 2 - 2);
  source->setProperty<PixelBoundaries>(0 , 0, image->getWidth(), image->getHeight());
  source->setProperty<IsophotalFlux>(10., 0., 1., 0.);
  source->setProperty<PixelCoordinateList>(pixel_coordinates);
  source->setProperty<DetectionFrame>(detection_frame);

  group.setProperty<PsfProperty>(*psf);
  group.setProperty<DetectionFrameGroupStamp>(image, image, PixelCoordinate(0,0), variance_image);

  auto task = PointModelFittingTask(100);
  task.computeProperties(group);

  auto point_model = source->getProperty<PointModelFitting>();
  BOOST_CHECK_CLOSE(10, point_model.getTotalFlux(), 0.1);
  BOOST_CHECK_LE(image->getWidth()/2 - 1.5, point_model.getX());
  BOOST_CHECK_GE(image->getWidth()/2 + 0.5, point_model.getX());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
