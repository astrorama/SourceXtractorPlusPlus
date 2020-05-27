/*
 * TestWCS.cpp
 *
 *  Created on: May 19, 2020
 *      Author: mschefer
 */


#include "ElementsKernel/ProgramHeaders.h"


#include <string>

#include <iostream>
#include <iomanip>

#include <fstream>
#include <tuple>
#include <vector>
#include <valarray>
#include <numeric>

#include <cstdlib>
#include <ctime>

#include <boost/any.hpp>
#include <boost/random.hpp>


#include <SEFramework/Image/BufferedImage.h>
#include <SEFramework/FITS/FitsImageSource.h>

#include <SEImplementation/CoordinateSystem/WCS.h>


namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace SourceXtractor;
//using namespace ModelFitting;
//using Euclid::make_unique;



class TestWCS : public Elements::Program {

public:

  po::options_description defineSpecificProgramOptions() override {
    po::options_description config_options { "TestWCS options" };

    // Add the specific program options
    config_options.add_options()
        ("image", po::value<std::string>()->required(), "fits image")
        ("hdu", po::value<int>()->default_value(0), "hdu")
        ("image_b", po::value<std::string>()->default_value(""), "fits image b")
        ("hdu_b", po::value<int>()->default_value(0), "hdu b")
        ("x", po::value<int>()->default_value(100), "x")
        ("y", po::value<int>()->default_value(100), "y")
        ;

    return config_options;
  }


  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {
    Elements::Logging logger = Elements::Logging::getLogger("TestWCS");

    auto filename = args["image"].as<std::string>();
    int hdu = args["hdu"].as<int>();

    auto filename_b = args["image_b"].as<std::string>();
    int hdu_b = args["hdu_b"].as<int>();

    int x = args["x"].as<int>();
    int y = args["y"].as<int>();

    auto image_source = std::make_shared<FitsImageSource<SeFloat>>(filename, hdu);
    auto image = BufferedImage<DetectionImage::PixelType>::create(image_source);
    auto coordinate_system = std::make_shared<WCS>(*image_source);

    //std::shared_ptr<WriteableImage<SeFloat>> target_image(WriteableBufferedImage<SeFloat>::create(target_image_source));

    logger.info("A -> A");
    auto wc = coordinate_system->imageToWorld(ImageCoordinate(x, y));
    std::cout << std::setprecision(9) << wc.m_alpha << " " << wc.m_delta << "\n";
    auto pc = coordinate_system->worldToImage(wc);
    std::cout << pc.m_x << " " << pc.m_y << "\n";

    if (filename_b != "") {

      for (int i=2; i<=37;i++) {
        auto image_source_b = std::make_shared<FitsImageSource<SeFloat>>(filename_b, i);
        //auto image_b = BufferedImage<DetectionImage::PixelType>::create(image_source_b);
        auto coordinate_system_b = std::make_shared<WCS>(*image_source_b);

        std::cout << ">>> " << image_source_b->getRepr() << "\n";

        logger.info("B -> B");

        auto wc = coordinate_system_b->imageToWorld(ImageCoordinate(x, y));
        std::cout << std::setprecision(9) << wc.m_alpha << " " << wc.m_delta << "\n";
        auto pc = coordinate_system_b->worldToImage(wc);
        std::cout << pc.m_x << " " << pc.m_y << "\n";

        logger.info("A -> B");

        wc = coordinate_system->imageToWorld(ImageCoordinate(x, y));
        std::cout << std::setprecision(9) << wc.m_alpha << " " << wc.m_delta << "\n";
        pc = coordinate_system_b->worldToImage(wc);
        std::cout << pc.m_x << " " << pc.m_y << "\n";

      }
    }


    logger.info("All done ^__^");
    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(TestWCS)
