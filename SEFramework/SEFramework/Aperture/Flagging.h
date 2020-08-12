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

#ifndef _SEFRAMEWORK_SEFRAMEWORK_APERTURE_FLAGGING_H
#define _SEFRAMEWORK_SEFRAMEWORK_APERTURE_FLAGGING_H

#include "SEFramework/Aperture/Aperture.h"
#include "SEFramework/Aperture/NeighbourInfo.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Source/SourceFlags.h"

namespace SourceXtractor {


Flags computeFlags(const std::shared_ptr<Aperture>& aperture,
                   SeFloat centroid_x, SeFloat centroid_y,
                   const std::vector<PixelCoordinate>& pix_list,
                   const std::shared_ptr<Image<SeFloat>>& detection_img,
                   const std::shared_ptr<Image<SeFloat>>& detection_variance,
                   const std::shared_ptr<Image<SeFloat>>& threshold_image,
                   SeFloat variance_threshold);

} // end SourceXtractor

#endif // _SEFRAMEWORK_SEFRAMEWORK_APERTURE_FLAGGING_H
