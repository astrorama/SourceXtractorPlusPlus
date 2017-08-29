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

#include "SEImplementation/CoordinateSystem/WCS.h"

namespace SExtractor {

using namespace wcslib;

WCS::WCS(const std::string& fits_file_path) {
  fitsfile *fptr = NULL;
  int status = 0;
  fits_open_file(&fptr, fits_file_path.c_str(), READONLY, &status);

  int hdutype;
  fits_get_hdu_type(fptr, &hdutype, &status);

  int nkeyrec;
  char* header;
  fits_hdr2str(fptr, 1, NULL, 0, &header, &nkeyrec, &status);

  if (hdutype == IMAGE_HDU) {
    int nreject = 0, nwcs = 0;
    wcsprm* wcs;
    wcspih(header, nkeyrec, WCSHDR_all, 0, &nreject, &nwcs, &wcs);

    m_wcs.reset(wcs);
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

}
