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
 * PsfPluginConfig.cpp
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón (greatly adapted from mschefer's code)
 */
#include <boost/algorithm/string.hpp>

#include "SEImplementation/Plugin/Psf/PsfPluginConfig.h"
#include "SEFramework/Psf/VariablePsf.h"
#include "SEFramework/Psf/VariablePsfStack.h"
#include "SEImplementation/Plugin/Psf/PsfTask.h"
#include <CCfits/CCfits>
#include <Configuration/ProgramOptionsHelper.h>
#include <ElementsKernel/Logging.h>

namespace po = boost::program_options;
using Euclid::Configuration::Configuration;

static auto logger = Elements::Logging::getLogger("PsfPlugin");

namespace fs = boost::filesystem;

namespace SourceXtractor {

static const std::string PSF_FILE{"psf-filename"};
static const std::string PSF_FWHM {"psf-fwhm" };
static const std::string PSF_PIXEL_SAMPLING {"psf-pixel-sampling" };

/*
 * Reading in a stacked PSF as it is being developed for co-added images in Euclid
 */
static std::shared_ptr<VariablePsfStack> readStackedPsf(std::unique_ptr<CCfits::FITS>& pFits) {
  logger.debug() << "Loading a PSF stack file.";
  std::shared_ptr<VariablePsfStack> act_stack = std::make_shared<VariablePsfStack>(std::move(pFits));
  return act_stack;
}

static std::shared_ptr<VariablePsf> readPsfEx(std::unique_ptr<CCfits::FITS> &pFits) {
  try {
    CCfits::ExtHDU& psf_data = pFits->extension("PSF_DATA");

    int n_components;
    psf_data.readKey("POLNAXIS", n_components);

    double pixel_sampling;
    psf_data.readKey("PSF_SAMP", pixel_sampling);

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
      throw Elements::Exception() << "Non square PSFEX format PSF (" << width << 'X' << height << ')';
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
      auto offset     = std::begin(raw_coeff_data) + i * n_pixels;
      coefficients[i] = VectorImage<SeFloat>::create(width, height, offset, offset + n_pixels);
    }

    logger.debug() << "Loaded variable PSF " << pFits->name() << " (" << width << ", " << height << ") with " << n_coeffs
                   << " coefficients";
    auto ll = logger.debug();
    ll << "Components: ";
    for (auto c : components) {
      ll << c.name << " ";
      if (PsfTask::component_value_getters.find(c.name) == PsfTask::component_value_getters.end()) {
        throw Elements::Exception() << "Can not find a getter for the component " << c.name;
      }
    }

    return std::make_shared<VariablePsf>(pixel_sampling, components, group_degrees, coefficients);
  } catch (CCfits::FITS::NoSuchHDU&) {
    throw;
  } catch (CCfits::FitsException &e) {
    throw Elements::Exception() << "Error loading PSFEx file: " << e.message();
  }
}

// templated to work around CCfits limitation that primary and extension HDUs are different classes
template<typename T>
static std::shared_ptr<VariablePsf> readImage(T& image_hdu) {
  double pixel_sampling;
  try {
    image_hdu.readKey("SAMPLING", pixel_sampling);
  }
  catch (CCfits::HDU::NoSuchKeyword&) {
    pixel_sampling = 1.;
  }

  if (image_hdu.axis(0) != image_hdu.axis(1)) {
    throw Elements::Exception() << "Non square image PSF (" << image_hdu.axis(0) << 'X'
                                << image_hdu.axis(1) << ')';
  }

  auto size = image_hdu.axis(0);

  std::valarray<double> data{};
  image_hdu.read(data);
  auto kernel = VectorImage<SeFloat>::create(size, size);
  std::copy(begin(data), end(data), kernel->getData().begin());

  logger.debug() << "Loaded image PSF(" << size << ", " << size << ") with sampling step " << pixel_sampling;

  return std::make_shared<VariablePsf>(pixel_sampling, kernel);
}

/// Reads a PSF from a fits file. The image must be square and have sides of odd
/// number of pixels.
std::shared_ptr<Psf> PsfPluginConfig::readPsf(const std::string& filename, int hdu_number) {

  // check whether the filename points to a delta function as PSF or NOPSF
  if (boost::to_upper_copy(fs::path(filename).filename().string())=="NOPSF") {
    return nullptr;
  }

  try {
    // Read the HDU from the file
    std::unique_ptr<CCfits::FITS> pFits{new CCfits::FITS(filename, CCfits::Read)};
    auto&                         image_hdu = pFits->pHDU();

    auto axes = image_hdu.axes();
    // PSF as image
    if (axes == 2) {
      if (hdu_number == 1) {
        return readImage(image_hdu);
      } else {
        auto& extension = pFits->extension(hdu_number - 1);
        return readImage(extension);
      }
    } else {
      try {
        // PSFEx format
        return readPsfEx(pFits);
      } catch (CCfits::FITS::NoSuchHDU& e) {
        logger.debug() << "Failed Reading a PsfEx file!";
        return readStackedPsf(pFits);
      }
    }
  } catch (CCfits::FitsException& e) {
    throw Elements::Exception() << "Error loading PSF file: " << e.message();
  }
}

std::shared_ptr<Psf> PsfPluginConfig::generateGaussianPsf(SeFloat fwhm, SeFloat pixel_sampling) {
  int size = int(fwhm / pixel_sampling + 1) * 6 + 1;
  auto kernel = VectorImage<SeFloat>::create(size, size);

  // convert full width half maximum to standard deviation
  double sigma_squared = (fwhm / (pixel_sampling * 2.355)) * (fwhm / (pixel_sampling * 2.355));

  double total = 0;
  for (int y = 0; y < size; y++) {
    for (int x = 0; x < size; x++) {
      double sx = (x - size / 2);
      double sy = (y - size / 2);
      double pixel_value = exp(-(sx * sx + sy * sy) / (2 * sigma_squared));
      total += pixel_value;
      kernel->setValue(x, y, pixel_value);
    }
  }
  for (int y = 0; y < size; y++) {
    for (int x = 0; x < size; x++) {
      kernel->setValue(x, y, kernel->getValue(x, y) / total);
    }
  }

  logger.info() << "Using gaussian PSF(" << fwhm << ") with pixel sampling step size " << pixel_sampling << " (size " << size << ")";

  return std::make_shared<VariablePsf>(pixel_sampling, kernel);
}

std::map<std::string, Configuration::OptionDescriptionList> PsfPluginConfig::getProgramOptions() {
  return {{"Variable PSF", {
    {PSF_FILE.c_str(), po::value<std::string>(),
        "Psf file (PSFEx/psfStack/image/NOPSF)."},
    {PSF_FWHM.c_str(), po::value<double>(),
       "Generate a gaussian PSF with the given full-width half-maximum (in pixels)"},
    {PSF_PIXEL_SAMPLING.c_str(), po::value<double>(),
        "Generate a gaussian PSF with the given pixel sampling step size"}
  }}};
}

void PsfPluginConfig::preInitialize(const Euclid::Configuration::Configuration::UserValues &args) {
  // Fail if there is no PSF file, but PSF_FWHM is set and PSF_PIXEL_SAMPLING is not
  if (args.find(PSF_FILE) == args.end() && args.find(PSF_FWHM) != args.end() &&
      args.find(PSF_PIXEL_SAMPLING) == args.end()) {
    throw Elements::Exception(PSF_PIXEL_SAMPLING + " is required when using " + PSF_FWHM);
  }
}

void PsfPluginConfig::initialize(const UserValues &args) {
  if (args.find(PSF_FILE) != args.end()) {
    auto psf_file = args.find(PSF_FILE)->second.as<std::string>();
    logger.debug() << "Provided by user: " << psf_file;
    if (boost::to_upper_copy(psf_file) == "NOPSF"){
      m_vpsf = nullptr;
    } else {
      m_vpsf = readPsf(args.find(PSF_FILE)->second.as<std::string>());
    }
  } else if (args.find(PSF_FWHM) != args.end()) {
    m_vpsf = generateGaussianPsf(args.find(PSF_FWHM)->second.as<double>(),
                                args.find(PSF_PIXEL_SAMPLING)->second.as<double>());
  }
}

const std::shared_ptr<Psf>& PsfPluginConfig::getPsf() const {
  return m_vpsf;
}

} // end SourceXtractor
