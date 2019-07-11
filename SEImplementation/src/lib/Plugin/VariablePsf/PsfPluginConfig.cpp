/*
 * PsfPluginConfig.cpp
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro ��lvarez Ayll��n (greatly adapted from mschefer's code)
 */

#include <Configuration/ProgramOptionsHelper.h>
#include <SEImplementation/Image/ImagePsf.h>
#include <CCfits/CCfits>
#include <ElementsKernel/Logging.h>
#include "SEImplementation/Plugin/Psf/PsfPluginConfig.h"
#include "SEImplementation/Plugin/Psf/PsfTask.h"
#include "SEFramework/Image/FitsWriter.h"

namespace po = boost::program_options;
using Euclid::Configuration::Configuration;

static auto logger = Elements::Logging::getLogger("PsfPlugin");

namespace SExtractor {

static const std::string PSF_FILE{"psf-filename"};
static const std::string PSF_FWHM {"psf-fwhm" };
static const std::string PSF_PIXELSCALE {"psf-pixel-scale" };

/*
 * Reading in a stacked PSF as it is being developed for co-added images in Euclid
 *
 * Some dummy code that tests the basic operations. Not yet operational, since
 * the VariablePsf class is not abstract and so on
 */
static std::shared_ptr<VariablePsf> readStackedPsf(std::unique_ptr<CCfits::FITS> &pFits) {

  // read in the stacked file;
  // extract one PSF;
  // write to a FITS file;
  std::shared_ptr<VectorImage<SeFloat>> act_psf;
  std::vector<double> pos_vector;
  std::shared_ptr<VariablePsfStack> act_stack = std::make_shared<VariablePsfStack>(std::move(pFits));
  pos_vector = {900.0, 1300.0};
  act_psf = act_stack->getPsf(pos_vector);
  FitsWriter::writeFile(*act_psf, "mypsf_1.fits");
  pos_vector = {15000.0, 4500.0};
  act_psf = act_stack->getPsf(pos_vector);
  FitsWriter::writeFile(*act_psf, "mypsf_2.fits");
  pos_vector = {10000.0, 12345.0};
  act_psf = act_stack->getPsf(pos_vector);
  FitsWriter::writeFile(*act_psf, "mypsf_3.fits");
  pos_vector = {16789.0, 70.0};
  act_psf = act_stack->getPsf(pos_vector);
  FitsWriter::writeFile(*act_psf, "mypsf_4.fits");
  pos_vector = {9876.0, 7654.0};
  act_psf = act_stack->getPsf(pos_vector);
  FitsWriter::writeFile(*act_psf, "mypsf_5.fits");
  pos_vector = {11843.0, 1528.0, 400.0};
  act_psf = act_stack->getPsf(pos_vector);
  FitsWriter::writeFile(*act_psf, "mypsf_6.fits");

  // thats kind of the emergency functionality
  return PsfPluginConfig::generateGaussianPsf(1.0, 0.1);
}

/*
 * Reading in a stacked PSF as it is being developed for co-added images in Euclid
 */
static std::shared_ptr<VariablePsf> readPsfEx(std::unique_ptr<CCfits::FITS> &pFits) {
  try {
    CCfits::ExtHDU &psf_data = pFits->extension("PSF_DATA");

    int n_components;
    psf_data.readKey("POLNAXIS", n_components);

    double pixel_scale;
    psf_data.readKey("PSF_SAMP", pixel_scale);

    std::vector<VariablePsf::Component> components(n_components);

    for (int i = 0; i < n_components; ++i) {
      auto index_str = std::to_string(i + 1);

      psf_data.readKey("POLGRP" + index_str, components[i].group_id);
      psf_data.readKey("POLNAME" + index_str, components[i].name);
      psf_data.readKey("POLZERO" + index_str, components[i].offset);
      psf_data.readKey("POLSCAL" + index_str, components[i].scale);

      // Groups in the FITS file are indexed starting at 1, but we use a zero-based index
      --components[i].group_id;
    }

    int n_groups;
    psf_data.readKey("POLNGRP", n_groups);

    std::vector<int> group_degrees(n_groups);

    for (int i = 0; i < n_groups; ++i) {
      auto index_str = std::to_string(i + 1);

      psf_data.readKey("POLDEG" + index_str, group_degrees[i]);
    }

    int width, height, n_coeffs, n_pixels;
    psf_data.readKey("PSFAXIS1", width);
    psf_data.readKey("PSFAXIS2", height);
    psf_data.readKey("PSFAXIS3", n_coeffs);

    if (width != height) {
      throw Elements::Exception() << "Non square PSF (" << width << 'X' << height << ')';
    }
    if (width % 2 == 0) {
      throw Elements::Exception() << "PSF kernel must have odd size";
    }

    n_pixels = width * height;

    std::vector<std::valarray<SeFloat>> all_data;
    psf_data.column("PSF_MASK").readArrays(all_data, 0, 0);
    auto& raw_coeff_data = all_data[0];

    std::vector<std::shared_ptr<VectorImage<SeFloat>>> coefficients(n_coeffs);

    for (int i = 0; i < n_coeffs; ++i) {
      auto offset = std::begin(raw_coeff_data) + i * n_pixels;
      coefficients[i] = VectorImage<SeFloat>::create(width, height, offset, offset + n_pixels);
    }

    logger.debug() << "Loaded variable PSF " << pFits->name() << " (" << width << ", " << height << ") with "
                   <<  n_coeffs << " coefficients";
    auto ll = logger.debug();
    ll << "Components: ";
    for (auto c : components) {
       ll << c.name << " ";
       if (component_value_getters.find(c.name) == component_value_getters.end()) {
         throw Elements::Exception() << "Can not find a getter for the component " << c.name;
       }
    }

    return std::make_shared<VariablePsf>(pixel_scale, components, group_degrees, coefficients);

  } catch (CCfits::FitsException &e) {
    throw Elements::Exception() << "Error loading PSFEx file: " << e.message();
  }
}

static std::shared_ptr<VariablePsf> readImage(CCfits::PHDU &image_hdu) {
  double pixel_scale;
  try {
    image_hdu.readKey("SCALE", pixel_scale);
  }
  catch (CCfits::HDU::NoSuchKeyword&) {
    logger.debug() << "No scale";
    pixel_scale = 1.;
  }

  if (image_hdu.axis(0) != image_hdu.axis(1)) {
    throw Elements::Exception() << "Non square PSF (" << image_hdu.axis(0) << 'X'
                                << image_hdu.axis(1) << ')';
  }

  auto size = image_hdu.axis(0);

  std::valarray<double> data{};
  image_hdu.read(data);
  auto kernel = VectorImage<SeFloat>::create(size, size);
  std::copy(begin(data), end(data), kernel->getData().begin());

  logger.debug() << "Loaded image PSF(" << size << ", " << size << ") with pixel scale " << pixel_scale;

  return std::make_shared<VariablePsf>(pixel_scale, kernel);
}

/// Reads a PSF from a fits file. The image must be square and have sides of odd
/// number of pixels. The pixel scale is read by the header keyword SCALE which
/// must be present
std::shared_ptr<VariablePsf> PsfPluginConfig::readPsf(const std::string &filename) {
  try {
    // Read the HDU from the file
    std::unique_ptr<CCfits::FITS> pFits{new CCfits::FITS(filename)};
    auto& image_hdu = pFits->pHDU();

    auto axes = image_hdu.axes();
    // PSF as image
    if (axes == 2) {
      return readImage(image_hdu);
    }
    // PSFEx format
    else {
      try {
        CCfits::ExtHDU &psf_data = pFits->extension("PSF_DATA");
	    return readPsfEx(pFits);
	} catch (CCfits::FitsException &e) {
	    return readStackedPsf(pFits);
	}
    }
  } catch (CCfits::FitsException &e) {
    throw Elements::Exception() << "Error loading PSF file: " << e.message();
  }
}

std::shared_ptr<VariablePsf> PsfPluginConfig::generateGaussianPsf(SeFloat fwhm, SeFloat pixel_scale) {
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

  logger.debug() << "Using gaussian PSF(" << fwhm << ") with pixel scale " << pixel_scale;

  return std::make_shared<VariablePsf>(pixel_scale, kernel);
}

std::map<std::string, Configuration::OptionDescriptionList> PsfPluginConfig::getProgramOptions() {
  return {{"Variable PSF", {
    {PSF_FILE.c_str(), po::value<std::string>(),
        "Psf image file (FITS format)."},
    {PSF_FWHM.c_str(), po::value<double>(),
       "Generate a gaussian PSF with given full-width half-maximum"},
    {PSF_PIXELSCALE.c_str(), po::value<double>(),
        "Generate a gaussian PSF with given full-width half-maximum"}
  }}};
}

void PsfPluginConfig::preInitialize(const Euclid::Configuration::Configuration::UserValues &args) {
  // Fail if there is no PSF file, but PSF_FWHM is set and PSF_PIXELSCALE is not
  if (args.find(PSF_FILE) == args.end() && args.find(PSF_FWHM) != args.end() &&
      args.find(PSF_PIXELSCALE) == args.end()) {
    throw Elements::Exception("psf-pixelscale is required when using psf-fwhm");
  }
}

void PsfPluginConfig::initialize(const UserValues &args) {
  if (args.find(PSF_FILE) != args.end()) {
    m_vpsf = readPsf(args.find(PSF_FILE)->second.as<std::string>());
  } else if (args.find(PSF_FWHM) != args.end()) {
    m_vpsf = generateGaussianPsf(args.find(PSF_FWHM)->second.as<double>(),
                                args.find(PSF_PIXELSCALE)->second.as<double>());
  }
}

const std::shared_ptr<VariablePsf>& PsfPluginConfig::getPsf() const {
  return m_vpsf;
}

} // end SExtractor
