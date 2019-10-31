/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file src/program/BenchBackgroundConvolution.cpp
 * @date 27/03/19
 * @author aalvarez
 */

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp>
#include <random>
#include "ElementsKernel/ProgramHeaders.h"
#include "ElementsKernel/Real.h"
#include "SEImplementation/Segmentation/BgConvolutionImageSource.h"
#include "SEImplementation/Segmentation/BgDFTConvolutionImageSource.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEUtils/IsClose.h"

#if BOOST_VERSION < 105600
#include <boost/units/detail/utility.hpp>
using boost::units::detail::demangle;
#else
using boost::core::demangle;
#endif

namespace po = boost::program_options;
namespace timer = boost::timer;
using namespace SourceXtractor;

static Elements::Logging logger = Elements::Logging::getLogger("BenchBackgroundConvolution");

class BenchBackgroundConvolution : public Elements::Program {
private:
  std::default_random_engine random_generator;
  std::uniform_real_distribution<SeFloat> random_dist{0, 1};

public:

  po::options_description defineSpecificProgramOptions() override {
    po::options_description options{};
    options.add_options()
      ("image-start", po::value<int>()->default_value(100), "Image start size")
      ("image-step-size", po::value<int>()->default_value(3), "Image step size")
      ("image-nsteps", po::value<int>()->default_value(1), "Number of steps for the image")
      ("kernel-start", po::value<int>()->default_value(3), "Kernel start size")
      ("kernel-step-size", po::value<int>()->default_value(4), "Kernel step size")
      ("kernel-nsteps", po::value<int>()->default_value(2), "Number of steps for the kernel")
      ("repeat", po::value<int>()->default_value(5), "Repeat")
      ("measures", po::value<int>()->default_value(10), "Number of measures");
    return options;
  }

  std::shared_ptr<VectorImage<SeFloat>> generateImage(int size) {
    auto img = VectorImage<SeFloat>::create(size, size);
    for (int x = 0; x < size; ++x) {
      for (int y = 0; y < size; ++y) {
        img->setValue(x, y, random_dist(random_generator));
      }
    }
    return img;
  }

  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {

    auto img_start = args["image-start"].as<int>();
    auto img_step_size = args["image-step-size"].as<int>();
    auto img_nsteps = args["image-nsteps"].as<int>();
    auto krn_start = args["kernel-start"].as<int>();
    auto krn_step_size = args["kernel-step-size"].as<int>();
    auto krn_nsteps = args["kernel-nsteps"].as<int>();
    auto repeat = args["repeat"].as<int>();
    auto measures = args["measures"].as<int>();

    std::cout << "Image,Kernel,Implementation,Time" << std::endl;

    for (int img_step = 0; img_step < img_nsteps; ++img_step) {
      auto img_size = img_start + img_step * img_step_size;
      auto image = generateImage(img_size);
      auto variance = generateImage(img_size);

      for (int krn_step = 0; krn_step < krn_nsteps; ++krn_step) {
        auto krn_size = krn_start + krn_step * krn_step_size;

        logger.info() << "Using an image of " << img_size << "x" << img_size;
        logger.info() << "Using a kernel of " << krn_size << "x" << krn_size;

        auto kernel = generateImage(krn_size);

        logger.info() << "Timing Direct implementation";
        auto direct_result = benchmark<BgConvolutionImageSource>(image, variance, kernel, repeat, measures);

        logger.info() << "Timing DFT implementation";
        auto dft_result = benchmark<BgDFTConvolutionImageSource>(image, variance, kernel, repeat, measures);

        logger.info() << "Comparing results";
        verifyResults(direct_result, dft_result);
      }
    }

    return Elements::ExitCode::OK;
  }

  template<typename BackgroundConvolution>
  std::shared_ptr<VectorImage<SeFloat>>
  benchmark(std::shared_ptr<VectorImage<SeFloat>>& image, std::shared_ptr<VectorImage<SeFloat>>& variance,
            std::shared_ptr<VectorImage<SeFloat>>& kernel, int repeat, int measures) {
    auto conv_name = demangle(typeid(BackgroundConvolution).name());

    auto bg_convolution = std::make_shared<BackgroundConvolution>(image, variance, 0.5, kernel);

    std::shared_ptr<VectorImage<SeFloat>> result;

    for (int m = 0; m < measures; ++m) {
      logger.info() << conv_name << " " << m + 1 << "/" << measures;
      timer::cpu_timer timer;
      timer.stop();

      for (int r = 0; r < repeat; ++r) {
        timer.start();
        result = bg_convolution->getImageTile(0, 0, image->getWidth(), image->getHeight())->getImage();
        timer.stop();
      }

      std::cout << image->getWidth() << ',' << kernel->getWidth() << ",\"" << conv_name << "\"," << timer.elapsed().wall
                << std::endl;
    }

    return result;
  }

  void verifyResults(std::shared_ptr<VectorImage<SeFloat>> a, std::shared_ptr<VectorImage<SeFloat>> b) {
    bool all_equal = true;
    for (int x = 0; x < a->getWidth(); ++x) {
      for (int y = 0; y < a->getHeight(); ++y) {
        auto av = a->getValue(x, y);
        auto bv = b->getValue(x, y);
        if (!isClose(av, bv, 1e-3, 1e-3)) {
          logger.info() << "Mismatch at " << x << ',' << y << ": "
                        << av << " != " << bv;
          all_equal = false;
        }
      }
    }
    if (all_equal) {
      logger.info() << "All elements are equal!";
    } else {
      logger.warn() << "Convoluted images are not equal!";
    }
  }
};

MAIN_FOR(BenchBackgroundConvolution)
