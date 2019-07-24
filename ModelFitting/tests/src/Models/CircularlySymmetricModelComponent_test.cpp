/*
 * CircularlySymmetricModelComponent_test.cpp
 *
 *  Created on: July 01, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#include <boost/test/unit_test.hpp>
#include <numeric>
#include "ModelFitting/Models/OnlySmooth.h"
#include "ModelFitting/Models/OldSharp.h"
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Models/CircularlySymmetricModelComponent.h"
#include "ModelFitting/utils.h"
#include "SEUtils/IsClose.h"

using namespace ModelFitting;
using namespace SExtractor;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (CircularlySymmetricModelComponent_test)

//-----------------------------------------------------------------------------

/**
 * Render a Sersic model, using only smooth, with the peak falling nicely into the center pixel.
 */
BOOST_AUTO_TEST_CASE (Sersic_smooth_test) {
  ManualParameter exp_i0{12.};
  ManualParameter exp_n{1.};
  ManualParameter exp_k{1.};
  SersicModelComponent sersic_model_component(make_unique<OnlySmooth>(), exp_i0, exp_n, exp_k);

  BOOST_CHECK(!sersic_model_component.insideSharpRegion(0, 0));
  BOOST_CHECK(!sersic_model_component.insideSharpRegion(10, 10));
  BOOST_CHECK(sersic_model_component.getSharpSampling().empty());

  BOOST_CHECK(isClose(sersic_model_component.getValue(0, 0), 12.));

  int w = 5, h = 5;
  std::vector<float> img(w * h);
  for (int x = 0; x < w; ++x) {
    for (int y = 0; y < h; ++y) {
      double dx = x - (w - 1) / 2.;
      double dy = y - (h - 1) / 2.;
      img[x + y * w] = sersic_model_component.getValue(dx, dy);
    }
  }
  std::vector<float> expected{
    0.70926896, 1.28253511, 1.62402340, 1.28253511, 0.70926896,
    1.28253511, 2.91740081, 4.41455329, 2.91740081, 1.28253511,
    1.62402340, 4.41455329, 12.00000000, 4.41455329, 1.62402340,
    1.28253511, 2.91740081, 4.41455329, 2.91740081, 1.28253511,
    0.70926896, 1.28253511, 1.62402340, 1.28253511, 0.70926896
  };
  BOOST_CHECK_EQUAL_COLLECTIONS(img.begin(), img.end(), expected.begin(), expected.end());
}

//-----------------------------------------------------------------------------

/**
 * Render a Sersic model, using old sharp.
 * The central area is oversampled, so the "missing flux" is spread across the central pixels
 */
BOOST_AUTO_TEST_CASE (Sersic_old_sharp_2_test) {
  ManualParameter exp_i0{12.};
  ManualParameter exp_n{1.};
  ManualParameter exp_k{1.};

  SersicModelComponent sersic_model_component(make_unique<OldSharp>(2.13), exp_i0, exp_n, exp_k);
  sersic_model_component.updateRasterizationInfo(1., 1.);

  BOOST_CHECK(sersic_model_component.insideSharpRegion( 0.0,  0.0));
  BOOST_CHECK(sersic_model_component.insideSharpRegion(-0.5, -0.5));
  BOOST_CHECK(sersic_model_component.insideSharpRegion( 1.0,  1.0));
  BOOST_CHECK(sersic_model_component.insideSharpRegion( 1.5,  1.5));
  BOOST_CHECK(!sersic_model_component.insideSharpRegion(2.0, 2.0));

  auto sampling = sersic_model_component.getSharpSampling();
  std::vector<double> sharp(9, 0.);

  for (auto& s: sampling) {
    ssize_t x = std::get<0>(s) + 3./2.;
    ssize_t y = std::get<1>(s) + 3./2.;
    if (x >= 0 && x < 3 && y >= 0 && y < 3) {
      sharp[x + y * 3] += std::get<2>(s);
    }
  }

  // The integral of this image should be smaller than the oversampled
  std::vector<double> smooth_only{
    2.91740081, 4.41455329, 2.91740081,
    4.41455329, 12.00000000, 4.41455329,
    2.91740081, 4.41455329, 2.91740081,
  };
  double smooth_integral = std::accumulate(smooth_only.begin(), smooth_only.end(), 0.);
  double sharp_integral = std::accumulate(sharp.begin(), sharp.end(), 0.);
  BOOST_CHECK_GT(sharp_integral, smooth_integral);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

//-----------------------------------------------------------------------------
