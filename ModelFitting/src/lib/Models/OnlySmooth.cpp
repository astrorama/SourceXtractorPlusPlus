/** 
 * @file OnlySmooth.cpp
 * @date September 1, 2015
 * @author Nikolaos Apostolakos
 */

#include "ModelFitting/Models/OnlySmooth.h"

namespace ModelFitting {

OnlySmooth::~OnlySmooth() = default;

void OnlySmooth::updateRasterizationInfo(double, double, ProfileProjection) {
  // do nothing here
}

bool OnlySmooth::insideSharpRegion(double) {
  return false;
}

std::pair<double, int> OnlySmooth::nextRadiusAndAngleNo(double) {
  return std::make_pair(0., 0);
}

} // end of namespace ModelFitting