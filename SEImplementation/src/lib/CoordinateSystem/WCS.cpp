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
 * WCS.cpp
 *
 *  Created on: Nov 17, 2016
 *      Author: mschefer
 */

#include <fitsio.h>

#include <wcslib/wcs.h>
#include <wcslib/wcshdr.h>
#include <wcslib/wcsfix.h>
#include <wcslib/wcsprintf.h>
#include <wcslib/getwcstab.h>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "SEImplementation/CoordinateSystem/WCS.h"
#include <boost/algorithm/string/trim.hpp>
#include <mutex>

namespace SourceXtractor {

static auto logger = Elements::Logging::getLogger("WCS");

decltype(&lincpy) safe_lincpy = &lincpy;

/**
 * wcslib < 5.18 is not fully safe thread, as some functions (like discpy, called by lincpy)
 * rely on global variables for determining the allocation sizes. For those versions, this is called
 * instead, wrapping the call with a mutex.
 */
static int wrapped_lincpy(int alloc, const struct linprm *linsrc, struct linprm *lindst) {
  static std::mutex lincpy_mutex;
  std::lock_guard<std::mutex> lock(lincpy_mutex);

  return lincpy(alloc, linsrc, lindst);
}


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
      wcsfree(wcs);
      wcsvfree(&nwcs_copy, &wcs);
    });
  }

  free(header);
  fits_close_file(fptr, &status);

  int wcsver[3];
  wcslib_version(wcsver);
  if (wcsver[0] < 5 || (wcsver[0] == 5 && wcsver[1] < 18)) {
    logger.info() << "wcslib " << wcsver[0] << "." << wcsver[1]
                  << " is not fully thread safe, using wrapped lincpy call!";
    safe_lincpy = &wrapped_lincpy;
  }
}

WCS::~WCS() {
}

WorldCoordinate WCS::imageToWorld(ImageCoordinate image_coordinate) const {
  // wcsprm is in/out, since its member lin is modified by wcsp2s
  wcsprm wcs_copy = *m_wcs;
  wcs_copy.lin.flag = -1;
  safe_lincpy(true, &m_wcs->lin, &wcs_copy.lin);
  linset(&wcs_copy.lin);

  // +1 as fits standard coordinates start at 1
  double pc_array[2] {image_coordinate.m_x + 1, image_coordinate.m_y + 1};

  double ic_array[2] {0, 0};
  double wc_array[2] {0, 0};
  double phi, theta;

  int status = 0;
  wcsp2s(&wcs_copy, 1, 1, pc_array, ic_array, &phi, &theta, wc_array, &status);
  linfree(&wcs_copy.lin);

  return WorldCoordinate(wc_array[0], wc_array[1]);
}

ImageCoordinate WCS::worldToImage(WorldCoordinate world_coordinate) const {
  // wcsprm is in/out, since its member lin is modified by wcss2p
  wcsprm wcs_copy = *m_wcs;
  wcs_copy.lin.flag = -1;
  safe_lincpy(true, &m_wcs->lin, &wcs_copy.lin);
  linset(&wcs_copy.lin);

  double pc_array[2] {0, 0};
  double ic_array[2] {0, 0};
  double wc_array[2] {world_coordinate.m_alpha, world_coordinate.m_delta};
  double phi, theta;

  int status = 0;
  wcss2p(&wcs_copy, 1, 1, wc_array, &phi, &theta, ic_array, pc_array, &status);
  linfree(&wcs_copy.lin);

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
