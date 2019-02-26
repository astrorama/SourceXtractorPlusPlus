/*
 * MoffatModelFitting_test.cpp
 *
 *  Created on: Jun 30, 2017
 *      Author: mschefer
 */

#include <boost/test/unit_test.hpp>

#include <memory>

#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFitting.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFittingTask.h"
#include "SEFramework/Source/SimpleSource.h"
#include "SEFramework/Source/SimpleSourceGroup.h"

#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStamp.h"
#include "SEImplementation/Plugin/Psf/PsfProperty.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h"
#include "SEFramework/Property/DetectionFrame.h"

#include "SEImplementation/Plugin/Psf/PsfPluginConfig.h"

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

struct MoffatModelFittingFixture {
  std::shared_ptr<SimpleSource> source {new SimpleSource};
  std::shared_ptr<MoffatModelFittingTask> model_fitting_task;

  MoffatModelFittingFixture() {
    model_fitting_task = std::make_shared<MoffatModelFittingTask>(100);
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (MoffatModelFitting_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(modelfitting_test, MoffatModelFittingFixture) {

  auto image = VectorImage<SeFloat>::create(20, 20);
  std::vector<PixelCoordinate> pixel_coordinates;

  for (auto x = 5; x < 16; ++x) {
    for (auto y = 5; y < 16; ++y) {
      pixel_coordinates.emplace_back(x, y);
      image->setValue(x, y, 100 / (0.01+sqrt((x-10.0)*(x-10.0)+(y-10.0)*(y-10.0))));
    }
  }


  auto variance_image = VectorImage<SeFloat>::create(image->getWidth(), image->getHeight());
  variance_image->fillValue(0);

  auto detection_frame = std::make_shared<DetectionImageFrame>(
    image, nullptr, 10, std::make_shared<DummyCoordinateSystem>(), 1, 65000, 1);

  source->setProperty<DetectionFrameSourceStamp>(image, image, PixelCoordinate(0,0), variance_image);
  source->setProperty<PixelCentroid>(13, 12);
  source->setProperty<ShapeParameters>(10, 10, 0, 0, 0, 0, 0, 0);
  source->setProperty<IsophotalFlux>(500., 0., 1., 0.);
  source->setProperty<PixelCoordinateList>(pixel_coordinates);
  source->setProperty<DetectionFrame>(detection_frame);

  model_fitting_task->computeProperties(*source);

  auto moffat_model = source->getProperty<MoffatModelFitting>();
  BOOST_CHECK(moffat_model.getIterations() > 0);
  BOOST_CHECK_CLOSE(moffat_model.getX(), 9.5, 1);
  BOOST_CHECK_CLOSE(moffat_model.getY(), 9.5, 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


