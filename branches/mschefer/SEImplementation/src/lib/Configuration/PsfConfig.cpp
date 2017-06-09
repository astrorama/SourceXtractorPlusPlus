/*
 * PsfConfig.cpp
 *
 *  Created on: May 23, 2017
 *      Author: mschefer
 */

#include "SEImplementation/Configuration/PsfConfig.h"
#include "SEFramework/Image/FitsReader.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

/// Reads a PSF from a fits file. The image must be square and have sides of odd
/// number of pixels. The pixel scale is read by the header keyword SCALE which
/// must be present
std::shared_ptr<ModelFitting::OpenCvPsf> readPsf(const std::string& filename) {
  try {
    // Read the HDU from the file
    std::unique_ptr<CCfits::FITS> pFits {new CCfits::FITS(filename)};
    auto& image_hdu = pFits->pHDU();
    // Get the pixel scale from the header
    double pixel_scale = 0.;

    try {
      image_hdu.readKey("SCALE", pixel_scale);
    } catch (CCfits::HDU::NoSuchKeyword& e) {
      pixel_scale = 1;
    }


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
    return std::make_shared<ModelFitting::OpenCvPsf>(pixel_scale, kernel);
  } catch (CCfits::FitsException& e) {
    throw Elements::Exception() << "Error loading PSF file: " << e.message();
  }
}


static const std::string PSF_FILE {"psf-file" };

PsfConfig::PsfConfig(long manager_id) :
    Configuration(manager_id) {}

std::map<std::string, Configuration::OptionDescriptionList> PsfConfig::getProgramOptions() {
  return { {"PSF", {
      {PSF_FILE.c_str(), po::value<std::string>(),
          "Psf image file (FITS format)."}  }}};
}

void PsfConfig::initialize(const UserValues& args) {
  if (args.find(PSF_FILE) != args.end()) {
    //m_psf = FitsReader<SeFloat>::readFile(args.find(PSF_FILE)->second.as<std::string>());
    m_psf = readPsf(args.find(PSF_FILE)->second.as<std::string>());
  }
}

} // SExtractor namespace



