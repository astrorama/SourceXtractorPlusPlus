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
 * WCS.h
 *
 *  Created on: Nov 17, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_COORDINATESYSTEM_WCS_H_
#define _SEFRAMEWORK_COORDINATESYSTEM_WCS_H_

#include <array>
#include <memory>
#include <map>
#include <string>

#include <wcslib/wcs.h>
#include <wcslib/wcshdr.h>

#include "SEFramework/CoordinateSystem/CoordinateSystem.h"
#include "SEFramework/FITS/FitsWcsSerializable.h"
#include "SEUtils/PixelCoordinate.h"

struct wcsprm;

namespace SourceXtractor {

/**
 * The parameters of a simple imaging WCS
 *
 * An alternative to a full set of FITS header cards, for callers that already
 * hold the values in a structured form.  Values follow the FITS conventions,
 * so crpix is 1-indexed; a caller whose source is 0-indexed converts first.
 */
struct ImagingWcsParams {
  std::array<double, 2> crpix;
  std::array<double, 2> crval;
  std::array<double, 2> cdelt;
  std::array<std::array<double, 2>, 2> pc;
  std::array<std::string, 2> ctype;
};

class WCS : public CoordinateSystem, public FitsWcsSerializable {
public:
  explicit WCS(const WCS& original);

  virtual ~WCS();

  /**
   * Build a WCS from a raw block of FITS header cards
   *
   * @param headers
   *    Concatenated 80-character header cards, as produced by
   *    FitsImageSource::getFitsHeaders
   * @param number_of_records
   *    The number of cards in that block
   */
  static std::shared_ptr<WCS> fromFitsHeaders(char* headers, int number_of_records);

  /**
   * Build a WCS from the parameters of a simple imaging WCS
   */
  static std::shared_ptr<WCS> fromImagingParams(const ImagingWcsParams& params);

  // Create a trivial WCS for a given number of axes
  static WCS identity(int naxis);

  WorldCoordinate imageToWorld(ImageCoordinate image_coordinate) const override;
  ImageCoordinate worldToImage(WorldCoordinate world_coordinate) const override;

  std::map<std::string, std::string> getFitsHeaders() const override;

  void addOffset(PixelCoordinate pc);

private:
  WCS(char* headers, int number_of_records);
  explicit WCS(const ImagingWcsParams& params);

  void initFits(char* headers, int number_of_records);
  void initImaging(const ImagingWcsParams& params);

  struct WcsprmDestroy {
    int nwcs;
    bool owned;

    void operator()(wcsprm* wcs) {
      if (!wcs)
        return;

      if (nwcs > 0) {
        wcsvfree(&nwcs, &wcs);
      } else {
        wcsfree(wcs);
        if (owned) {
          delete wcs;
        }
      }
    }
  };

  using WcsprmPtr = std::unique_ptr<wcsprm, WcsprmDestroy>;

  WcsprmPtr m_wcs;

  static WcsprmPtr make_wcsprm_ptr();
  static WcsprmPtr make_wcsprm_ptr(wcsprm* wcs);
  static WcsprmPtr make_wcsprm_ptr(wcsprm* wcs, bool owned);
  static WcsprmPtr make_wcsprm_ptr(wcsprm* wcs, bool owned, int nwcs);

  explicit WCS(WcsprmPtr wcs)
    : m_wcs(std::move(wcs)) {}

};

}

#endif /* _SEFRAMEWORK_COORDINATESYSTEM_WCS_H_ */
