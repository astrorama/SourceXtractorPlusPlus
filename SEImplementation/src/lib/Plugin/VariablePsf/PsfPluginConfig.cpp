/*
 * PsfPluginConfig.cpp
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón (greatly adapted from mschefer's code)
 */

#include <Configuration/ProgramOptionsHelper.h>
#include <SEImplementation/Image/ImagePsf.h>
#include <CCfits/FITS.h>
#include <CCfits/PHDU.h>
#include <CCfits/Column.h>
#include <ElementsKernel/Logging.h>
#include "SEImplementation/Plugin/Psf/PsfPluginConfig.h"

namespace po = boost::program_options;
using Euclid::Configuration::Configuration;

static auto logger = Elements::Logging::getLogger("PsfPlugin");

namespace SExtractor {

static const std::string VAR_PSF_FILE{"var-psf-file"};


static std::shared_ptr<VariablePsf> readPsfEx(const std::string &filename) {
  try {
    std::unique_ptr<CCfits::FITS> pFits{new CCfits::FITS(filename)};
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
    auto &raw_coeff_data = all_data[0];

    std::vector<std::shared_ptr<VectorImage<SeFloat>>> coefficients(n_coeffs);

    for (int i = 0; i < n_coeffs; ++i) {
      auto offset = std::begin(raw_coeff_data) + i * n_pixels;
      coefficients[i] = VectorImage<SeFloat>::create(width, height, offset, offset + n_pixels);
    }

    logger.debug() << "Loaded variable PSF(" << width << ", " << height << ") with "
                   <<  n_coeffs << " coefficients";
    auto ll = logger.debug();
    ll << "Components: ";
    for (auto c : components) {
       ll << c.name << " ";
    }

    return std::make_shared<VariablePsf>(pixel_scale, components, group_degrees, coefficients);

  } catch (CCfits::FitsException &e) {
    throw Elements::Exception() << "Error loading PSFEx file: " << e.message();
  }
}


std::map<std::string, Configuration::OptionDescriptionList> PsfPluginConfig::getProgramOptions() {
  return {{"Variable PSF", {
    {VAR_PSF_FILE.c_str(), po::value<std::string>(),
        "Psf image file (FITS format)."}
  }}};
};

void PsfPluginConfig::initialize(const UserValues &args) {
  if (args.find(VAR_PSF_FILE) != args.end()) {
    m_vpsf = readPsfEx(args.find(VAR_PSF_FILE)->second.as<std::string>());
  }
}

const std::shared_ptr<VariablePsf>& PsfPluginConfig::getPsf() const {
  return m_vpsf;
}

} // end SExtractor
