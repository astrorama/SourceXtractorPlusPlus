/*
 * WCS.cpp
 *
 *  Created on: Nov 17, 2016
 *      Author: mschefer
 */

#include <fitsio.h>

namespace wcslib {

#include <wcslib/wcs.h>
#include <wcslib/wcshdr.h>
#include <wcslib/wcsfix.h>
#include <wcslib/wcsprintf.h>
#include <wcslib/getwcstab.h>

}

#include "ElementsKernel/Exception.h"
#include "SEImplementation/CoordinateSystem/WCS.h"
#include <boost/algorithm/string/trim.hpp>

namespace SExtractor {

using namespace wcslib;

WCS::WCS(const std::string& fits_file_path, int hdu_number) : m_wcs(nullptr, nullptr) {
  fitsfile *fptr = NULL;
  int status = 0;
  fits_open_file(&fptr, fits_file_path.c_str(), READONLY, &status);

  int hdu_type;
  fits_movabs_hdu(fptr, hdu_number, &hdu_type, &status);

  if (status != 0 || hdu_type != IMAGE_HDU) {
    throw Elements::Exception() << "Can't read WCS information from " << fits_file_path << " HDU " << hdu_number;
  }

  int nkeyrec;
  char* header;
  fits_hdr2str(fptr, 1, NULL, 0, &header, &nkeyrec, &status);

  if (hdu_type == IMAGE_HDU) {
    int nreject = 0, nwcs = 0;
    wcsprm* wcs;
    wcspih(header, nkeyrec, WCSHDR_all, 0, &nreject, &nwcs, &wcs);
    wcsset(wcs);

    m_wcs = decltype(m_wcs)(wcs, [nwcs](wcsprm* wcs) {
      int nwcs_copy = nwcs;
      wcsvfree(&nwcs_copy, &wcs);
    });
  }

  free(header);
  fits_close_file(fptr, &status);
}

WCS::~WCS() {
}

WorldCoordinate WCS::imageToWorld(ImageCoordinate image_coordinate) const {

  // +1 as fits standard coordinates start at 1
  double pc_array[2] {image_coordinate.m_x + 1, image_coordinate.m_y + 1};

  double ic_array[2] {0, 0};
  double wc_array[2] {0, 0};
  double phi, theta;

  int status = 0;
  wcsp2s(m_wcs.get(), 1, 1, pc_array, ic_array, &phi, &theta, wc_array, &status);

  return WorldCoordinate(wc_array[0], wc_array[1]);
}

ImageCoordinate WCS::worldToImage(WorldCoordinate world_coordinate) const {
  double pc_array[2] {0, 0};
  double ic_array[2] {0, 0};
  double wc_array[2] {world_coordinate.m_alpha, world_coordinate.m_delta};
  double phi, theta;

  int status = 0;
  wcss2p(m_wcs.get(), 1, 1, wc_array, &phi, &theta, ic_array, pc_array, &status);

  return ImageCoordinate(pc_array[0] - 1, pc_array[1] - 1); // -1 as fits standard coordinates start at 1
}

std::map<std::string, std::string> WCS::getFitsHeaders() const {
  int nkeyrec;
  char *raw_header;

  if (wcshdo(WCSHDO_none, m_wcs.get(), &nkeyrec, &raw_header) != 0) {
    throw Elements::Exception() << "Failed to get the FITS headers for the WCS coordinate system";
  }

  std::map<std::string, std::string> headers;
  for (int i = 0; i < nkeyrec; ++i) {
    char *hptr = &raw_header[80 * i];
    std::string key(hptr, hptr + 8);
    boost::trim(key);
    std::string value(hptr + 9, hptr + 72);
    boost::trim(value);
    if (!key.empty()) {
      headers.emplace(std::make_pair(key, value));
    }
  }

  free(raw_header);
  return headers;
}

}
