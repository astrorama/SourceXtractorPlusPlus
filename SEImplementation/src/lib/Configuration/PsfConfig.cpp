/*
 * PsfConfig.cpp
 *
 *  Created on: May 23, 2017
 *      Author: mschefer
 */

#include <iostream>
#include <CCfits/CCfits>

#include "SEImplementation/Configuration/PsfConfig.h"
#include "SEFramework/Image/FitsReader.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {


///// Writes an OpenCv Mat to an image FITS file (prepend the filename with '!' to
///// override existing files)
//void writeToFits(const cv::Mat& image, const std::string& filename) {
//  std::valarray<double> data (image.total());
//  std::copy(image.begin<double>(), image.end<double>(), begin(data));
//  long naxis = 2;
//  long naxes[2] = {image.size[1], image.size[0]};
//  std::unique_ptr<CCfits::FITS> pFits {new CCfits::FITS("!"+filename, DOUBLE_IMG, naxis, naxes)};
//  pFits->pHDU().write(1, image.total(), data);
//  std::cout << "Created file " << filename << '\n';
//}
//
static const std::string PSF_FILE {"psf-file" };
static const std::string PSF_FWHM {"psf-fwhm" };
static const std::string PSF_PIXELSCALE {"psf-pixelscale" };

/// Reads a PSF from a fits file. The image must be square and have sides of odd
/// number of pixels. The pixel scale is read by the header keyword SCALE which
/// must be present
std::shared_ptr<ImagePsf> PsfConfig::readPsf(const std::string& filename) {
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
      std::cout << "no scale\n";

    }

    auto axes = image_hdu.axes();
    if (axes == 2) {
      // PSF as image

      // Get the dimension of the image
      if (image_hdu.axis(0) != image_hdu.axis(1)) {
        throw Elements::Exception() << "Non square PSF (" << image_hdu.axis(0) << 'X'
                                    << image_hdu.axis(1) << ')';
      }

      auto size = image_hdu.axis(0);
      // Get the data
      std::valarray<double> data {};
      image_hdu.read(data);
      auto kernel = VectorImage<SeFloat>::create(size, size);
      std::copy(begin(data), end(data), kernel->getData().begin());
      std::cout << "pixel scale: " << pixel_scale << std::endl;
      return std::make_shared<ImagePsf>(pixel_scale, kernel);
    } else {

      // PSFEx format

      CCfits::ExtHDU& psf_data = pFits->extension("PSF_DATA");

      int size;
      psf_data.readKey("PSFAXIS1", size);

      // Get the data
      std::valarray<double> data {};
      psf_data.readKey("PSF_SAMP", pixel_scale);
      psf_data.column("PSF_MASK").read(data, 1);

      auto kernel = VectorImage<SeFloat>::create(size, size);
      std::copy(begin(data), end(data), kernel->getData().begin());
      std::cout << "pixel scale: " << pixel_scale << std::endl;
      //writeToFits(kernel, "testpsf.fits");
      return std::make_shared<ImagePsf>(pixel_scale, kernel);
    }
  } catch (CCfits::FitsException& e) {
    throw Elements::Exception() << "Error loading PSF file: " << e.message();
  }
}

std::shared_ptr<ImagePsf> PsfConfig::generateGaussianPsf(SeFloat fwhm, SeFloat pixel_scale) {
  int supersample = 10;
  int size = int(fwhm / pixel_scale + 1) * 6 + 1;
  auto kernel = VectorImage<SeFloat>::create(size, size);

  // convert full width half maximum to standard deviation
  double sigma_squared = (fwhm / (pixel_scale * 2.355)) * (fwhm / (pixel_scale * 2.355));

  double total = 0;
  for (int y=0; y < size; y++) {
    for (int x=0; x < size; x++) {

      double pixel_value = 0.0;
      for (int iy = -(supersample/2); iy <= supersample/2; iy++) {
        for (int ix = -(supersample/2); ix <=  supersample/2; ix++) {
          double sx = (x-size/2) + ix * (1.0 / supersample);
          double sy = (y-size/2) + iy * (1.0 / supersample);
          auto sub_pixel_value = exp(- (sx*sx + sy*sy) / (2 * sigma_squared) );
          pixel_value += sub_pixel_value;
        }
      }
      pixel_value /= (supersample * supersample);
      total += pixel_value;
      kernel->setValue(x, y, pixel_value);
    }
  }
  for (int y=0; y < size; y++) {
    for (int x=0; x < size; x++) {
      kernel->setValue(x, y, kernel->getValue(x, y) / total);
    }
  }

  return std::make_shared<ImagePsf>(pixel_scale, kernel);
}

PsfConfig::PsfConfig(long manager_id) :
    Configuration(manager_id), m_psf(nullptr) {}

std::map<std::string, Configuration::OptionDescriptionList> PsfConfig::getProgramOptions() {
  return { {"PSF", {
      {PSF_FILE.c_str(), po::value<std::string>(),
          "Psf image file (FITS format)."},
      {PSF_FWHM.c_str(), po::value<double>(),
          "Generate a gaussian PSF with given full-width half-maximum"},
      {PSF_PIXELSCALE.c_str(), po::value<double>()->default_value(.2),
          "Pixel scale for generated PSF"}}}};
}

void PsfConfig::initialize(const UserValues& args) {
  if (args.find(PSF_FILE) != args.end()) {
    m_psf = readPsf(args.find(PSF_FILE)->second.as<std::string>());
  } else if (args.find(PSF_FWHM) != args.end()) {
    m_psf =
        generateGaussianPsf(args.find(PSF_FWHM)->second.as<double>(), args.find(PSF_PIXELSCALE)->second.as<double>());
  }
}

} // SExtractor namespace



