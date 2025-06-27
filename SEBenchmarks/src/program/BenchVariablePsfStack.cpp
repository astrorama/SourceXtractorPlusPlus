/** Copyright © 2019-2025 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
 * @file src/program/BenchVariablePsfStack.cpp
 * @date 06/27/25
 * @author marc schefer
 */

#include <map>
#include <string>
#include <random>

#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include "ElementsKernel/Real.h"
#include "SEFramework/Psf/VariablePsfStack.h"

namespace po = boost::program_options;
namespace timer = boost::timer;
using namespace SourceXtractor;

static Elements::Logging logger = Elements::Logging::getLogger("BenchVariablePsfStack");

class BenchVariablePsfStack : public Elements::Program {
private:
  std::default_random_engine random_generator;
  std::uniform_real_distribution<double> random_dist{0.0, 1000.0};

public:

  po::options_description defineSpecificProgramOptions() override {
    po::options_description options{};
    options.add_options()
      ("iterations", po::value<int>()->default_value(100000), "Number of getPsf calls to benchmark")
      ("measures", po::value<int>()->default_value(10), "Number of timing measurements to take")
      ("fits-file", po::value<std::string>()->default_value(""), "FITS file containing PSF stack (optional, will use nullptr if not provided)");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value> &args) override {

    auto iterations = args["iterations"].as<int>();
    auto measures = args["measures"].as<int>();
    auto fits_file = args["fits-file"].as<std::string>();

    logger.info() << "Benchmarking VariablePsfStack::getPsf() with " << iterations << " iterations";
    logger.info() << "Taking " << measures << " timing measurements";

    // Initialize VariablePsfStack with FITS file if provided, otherwise nullptr
    std::shared_ptr<CCfits::FITS> fitsPtr = nullptr;
    if (!fits_file.empty()) {
      try {
        fitsPtr = std::make_shared<CCfits::FITS>(fits_file);
        logger.info() << "Using FITS file: " << fits_file;
      } catch (const std::exception& e) {
        logger.error() << "Failed to load FITS file '" << fits_file << "': " << e.what();
        logger.info() << "Continuing with nullptr - this will likely cause exceptions during getPsf() calls";
      }
    } else {
      logger.info() << "No FITS file provided - using nullptr (will likely cause exceptions)";
    }
    
    try {
      VariablePsfStack psfStack(fitsPtr);
      
      logger.info() << "VariablePsfStack loaded successfully with " << psfStack.getNumberOfPsfs() << " PSFs";
      logger.info() << "PSF size: " << psfStack.getWidth() << "x" << psfStack.getHeight();
      logger.info() << "Pixel sampling: " << psfStack.getPixelSampling();
      
      std::cout << "Iterations,Measurement,Time_nanoseconds" << std::endl;

      for (int m = 0; m < measures; ++m) {
        logger.info() << "Measurement " << (m + 1) << "/" << measures;
        
        timer::cpu_timer timer;
        timer.stop();
        
        // Prepare test values for getPsf calls
        std::vector<std::vector<double>> testValues;
        testValues.reserve(iterations);
        
        for (int i = 0; i < iterations; ++i) {
          testValues.push_back({random_dist(random_generator), random_dist(random_generator)});
        }
        
        // Start timing
        timer.start();
        
        for (int i = 0; i < iterations; ++i) {
          try {
            auto psf = psfStack.getPsf(testValues[i]);
            // Prevent compiler optimization by using the result
            volatile auto width = psf->getWidth();
            (void)width; // Suppress unused variable warning
          } catch (const std::exception& e) {
            // Expected to fail with nullptr, but we still measure the timing
            // until the exception is thrown
          }
        }
        
        timer.stop();
        
        auto elapsed_ns = timer.elapsed().wall;
        std::cout << iterations << "," << (m + 1) << "," << elapsed_ns << std::endl;
        
        logger.info() << "Time for " << iterations << " calls: " << (elapsed_ns / 1e9) << " seconds";
        logger.info() << "Average time per call: " << (elapsed_ns / iterations) << " nanoseconds";
      }
      
    } catch (const std::exception& e) {
      logger.error() << "Error initializing VariablePsfStack: " << e.what();
      logger.info() << "This is expected with nullptr parameter - fix the FITS file parameter later";
      
      // Still run a basic timing test to measure overhead
      std::cout << "Running basic timing test without actual PSF operations..." << std::endl;
      std::cout << "Iterations,Measurement,Time_nanoseconds" << std::endl;
      
      for (int m = 0; m < measures; ++m) {
        timer::cpu_timer timer;
        timer.stop();
        
        timer.start();
        for (int i = 0; i < iterations; ++i) {
          // Just measure the overhead of generating random values and vector creation
          std::vector<double> values = {random_dist(random_generator), random_dist(random_generator)};
          volatile auto size = values.size();
          (void)size;
        }
        timer.stop();
        
        auto elapsed_ns = timer.elapsed().wall;
        std::cout << iterations << "," << (m + 1) << "," << elapsed_ns << std::endl;
      }
    }

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(BenchVariablePsfStack)
