/**
 * Copyright © 2019-2022 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include "SEFramework/CoordinateSystem/WCS.h"

#include <boost/algorithm/string/trim.hpp>
#include <fitsio.h>
#include <mutex>
#include <wcslib/dis.h>
#include <wcslib/wcs.h>
#include <wcslib/wcsfix.h>
#include <wcslib/wcshdr.h>
#include <wcslib/wcsprintf.h>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

namespace SourceXtractor {

static auto logger = Elements::Logging::getLogger("WCS");

decltype(&wcssub) safe_wcssub = &wcssub;

/**
 * Translate the return code from wcspih to an elements exception
 */
static void wcsRaiseOnParseError(int ret_code) {
  switch (ret_code) {
    case WCSHDRERR_SUCCESS:
      break;
    case WCSHDRERR_MEMORY:
      throw Elements::Exception() << "wcslib failed to allocate memory";
    case WCSHDRERR_PARSER:
      throw Elements::Exception() << "wcslib failed to parse the FITS headers";
    default:
      throw Elements::Exception() << "Unexpected error when parsing the FITS WCS header: "
                                  << ret_code;
  }
}

static void wcsLogErr(wcserr *err) {
  if (err) {
    logger.error() << err->file << ":" << err->line_no << " " << err->function;
    logger.error() << err->msg;
  }
}

/**
 * Translate the return code from wcss2p and wcsp2s to an elements exception
 */
static void wcsRaiseOnTransformError(wcsprm *wcs, int ret_code) {
  if (ret_code != WCSERR_SUCCESS) {
    wcsLogErr(wcs->err);
    wcsLogErr(wcs->lin.err);
    if (wcs->lin.dispre) {
      wcsLogErr(wcs->lin.dispre->err);
    }
    if (wcs->lin.disseq) {
      wcsLogErr(wcs->lin.disseq->err);
    }
    throw InvalidCoordinatesException() << wcs_errmsg[ret_code];
  }
}

/**
 * Extract the keywords from the headers
 */
static std::set<std::string> wcsExtractKeywords(const char *header, int number_of_records) {
  std::set<std::string> result;
  for (const char *p = header; *p != '\0' && number_of_records; --number_of_records, p += 80) {
    size_t len = strcspn(p, "= ");
    result.insert(std::string(p, len));
  }
  return result;
}

/**
 * Look for some known inconsistencies that wcslib will not report
 */
static void wcsCheckHeaders(const wcsprm *wcs, const char *headers_str, int number_of_records) {
  auto headers = wcsExtractKeywords(headers_str, number_of_records);

  // SIP present, but not specified in CTYPE
  // See https://github.com/astrorama/SourceXtractorPlusPlus/issues/254#issuecomment-765235869
  if (wcs->lin.dispre) {
    bool sip_used = false, sip_specified = false;
    for (int i = 0; i < wcs->naxis; ++i) {
      sip_used |= (strncmp(wcs->lin.dispre->dtype[i], "SIP", 3) == 0);
      size_t ctype_len = strlen(wcs->ctype[i]);
      sip_specified |= (strncmp(wcs->ctype[i] + ctype_len - 4, "-SIP", 4) == 0);
    }
    if (sip_used && !sip_specified) {
      logger.warn() << "SIP coefficients present, but CTYPE has not the '-SIP' suffix";
      logger.warn() << "SIP distortion will be applied, but this may not be desired";
      logger.warn() << "To suppress this warning, explicitly add the '-SIP' suffix to the CTYPE,";
      logger.warn() << "or remove the SIP distortion coefficients";
    }
  }
}

/**
 * Wrap wcslib error report and display them via logger
 */
static void wcsReportWarnings(const char *err_buffer) {
  if (err_buffer[0]) {
    logger.warn() << "WCS generated some errors in strict mode. This may be OK.";
    logger.warn() << "Will run in relaxed mode.";
    const char *eol;
    do {
      eol = strchr(err_buffer, '\n');
      if (eol) {
        logger.warn() << std::string(err_buffer, eol - err_buffer);
        err_buffer = eol + 1;
      }
      else {
        logger.warn() << std::string(err_buffer);
      }
    } while (eol);
  }
}

/**
 * wcslib < 5.18 is not fully safe thread, as some functions (like discpy, called by lincpy)
 * rely on global variables for determining the allocation sizes. For those versions, this is called
 * instead, wrapping the call with a mutex.
 */
static int wrapped_wcssub(int alloc, const struct wcsprm* wcssrc, int* nsub, int axes[], struct wcsprm* wcsdst) {
  static std::mutex           cpy_mutex;
  std::lock_guard<std::mutex> lock(cpy_mutex);

  return wcssub(alloc, wcssrc, nsub, axes, wcsdst);
}

WCS::WCS(const FitsImageSource& fits_image_source) : m_wcs(nullptr, nullptr) {
  int number_of_records = 0;
  auto fits_headers = fits_image_source.getFitsHeaders(number_of_records);

  init(&(*fits_headers)[0], number_of_records);
}

WCS::WCS(const WCS& original) : m_wcs(nullptr, nullptr) {

  //FIXME Horrible hack: I couldn't figure out how to properly do a deep copy wcsprm so instead
  // of making a copy, I use the ascii headers output from the original to recreate a new one

  int number_of_records;
  char *raw_header;

  if (wcshdo(WCSHDO_none, original.m_wcs.get(), &number_of_records, &raw_header) != 0) {
    throw Elements::Exception() << "Failed to get the FITS headers for the WCS coordinate system when copying WCS";
  }

  init(raw_header, number_of_records);

  free(raw_header);
}


void WCS::init(char* headers, int number_of_records) {
  wcserr_enable(1);

  int nreject = 0, nwcs = 0, nreject_strict = 0;
  wcsprm* wcs;

  // Write warnings to a buffer
  wcsprintf_set(nullptr);

  // Do a first pass, in strict mode, and ignore the result.
  // Log the reported errors as warnings
  int ret = wcspih(headers, number_of_records, WCSHDR_strict, 2, &nreject_strict, &nwcs, &wcs);
  wcsRaiseOnParseError(ret);
  wcsReportWarnings(wcsprintf_buf());

  // Do a second pass, in relaxed mode. We use the result.
  ret = wcspih(headers, number_of_records, WCSHDR_all, 0, &nreject, &nwcs, &wcs);
  wcsRaiseOnParseError(ret);
  wcsset(wcs);

  // There are some things worth reporting about which WCS will not necessarily complain
  wcsCheckHeaders(wcs, headers, number_of_records);

  m_wcs = decltype(m_wcs)(wcs, [nwcs](wcsprm* ptr) {
    int nwcs_copy = nwcs;
    wcsfree(ptr);
    wcsvfree(&nwcs_copy, &ptr);
  });

  int wcsver[3];
  wcslib_version(wcsver);
  if (wcsver[0] < 5 || (wcsver[0] == 5 && wcsver[1] < 18)) {
    logger.info() << "wcslib " << wcsver[0] << "." << wcsver[1]
                  << " is not fully thread safe, using wrapped lincpy call!";
    safe_wcssub = &wrapped_wcssub;
  }
}


WCS::~WCS() {
}

WorldCoordinate WCS::imageToWorld(ImageCoordinate image_coordinate) const {
  // wcsprm is in/out
  wcsprm wcs_copy;
  wcs_copy.flag = -1;
  safe_wcssub(true, m_wcs.get(), nullptr, nullptr, &wcs_copy);

  // +1 as fits standard coordinates start at 1
  double pc_array[2] {image_coordinate.m_x + 1, image_coordinate.m_y + 1};

  double ic_array[2] {0, 0};
  double wc_array[2] {0, 0};
  double phi, theta;

  int status = 0;
  int ret_val = wcsp2s(&wcs_copy, 1, 1, pc_array, ic_array, &phi, &theta, wc_array, &status);
  wcsRaiseOnTransformError(&wcs_copy, ret_val);
  wcsfree(&wcs_copy);

  return WorldCoordinate(wc_array[0], wc_array[1]);
}

ImageCoordinate WCS::worldToImage(WorldCoordinate world_coordinate) const {
  // wcsprm is in/out
  wcsprm wcs_copy;
  wcs_copy.flag = -1;
  safe_wcssub(true, m_wcs.get(), nullptr, nullptr, &wcs_copy);

  double pc_array[2] {0, 0};
  double ic_array[2] {0, 0};
  double wc_array[2] {world_coordinate.m_alpha, world_coordinate.m_delta};
  double phi, theta;

  int status = 0;
  int ret_val = wcss2p(&wcs_copy, 1, 1, wc_array, &phi, &theta, ic_array, pc_array, &status);
  wcsRaiseOnTransformError(&wcs_copy, ret_val);
  wcsfree(&wcs_copy);

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

void WCS::addOffset(PixelCoordinate pc) {
  m_wcs->crpix[0] -= pc.m_x;
  m_wcs->crpix[1] -= pc.m_y;
}


}
