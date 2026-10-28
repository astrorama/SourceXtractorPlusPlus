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
 * GWCS.h
 *
 *  Created on: Sep 23, 2026
 *      Author: embray
 */

#ifndef _SEFRAMEWORK_COORDINATESYSTEM_GWCS_H_
#define _SEFRAMEWORK_COORDINATESYSTEM_GWCS_H_

#include <memory>

#include <asdf/gwcs/gwcs.h>

#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace SourceXtractor {

/**
 * A generalized WCS, evaluated through libasdf-gwcs
 *
 * Unlike WCS this is not restricted to the transforms FITS can express, so it
 * has no FITS header representation and does not implement FitsWcsSerializable
 * for the moment (it might later if we implement ASDF-in-FITS).
 *
 * Safe to share between threads: the underlying evaluation context belongs to
 * the thread that created it, so each conversion evaluates on a private
 * context of its own--asdf_gwcs_eval_copy going forwards and
 * asdf_gwcs_eval_invert going back--in the same spirit as WCS copying its
 * wcsprm per call.
 */
class GWCS : public CoordinateSystem {
public:
  using EvalPtr = std::unique_ptr<asdf_gwcs_eval_t, decltype(&asdf_gwcs_eval_destroy)>;

  /**
   * @param eval
   *    The evaluation context, which must not be null
   */
  explicit GWCS(EvalPtr eval);

  virtual ~GWCS();

  WorldCoordinate imageToWorld(ImageCoordinate image_coordinate) const override;

  /**
   * @throws InvalidCoordinatesException
   *    if the WCS cannot be inverted at all, or if the given sky position has
   *    no image coordinates--being outside the projection's valid region.
   *    Callers treat the latter as "this source is not on this frame".
   */
  ImageCoordinate worldToImage(WorldCoordinate world_coordinate) const override;

private:
  EvalPtr m_eval;
};

}  // namespace SourceXtractor

#endif /* _SEFRAMEWORK_COORDINATESYSTEM_GWCS_H_ */
