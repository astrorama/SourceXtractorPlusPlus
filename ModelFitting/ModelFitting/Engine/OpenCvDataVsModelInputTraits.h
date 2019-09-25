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
/** 
 * @file OpenCvDataVsModelInputTraits.h
 * @date September 2, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_OPENCVDATAVSMODELINPUTTRAITS_H
#define	MODELFITTING_OPENCVDATAVSMODELINPUTTRAITS_H

#include <utility>
#include <opencv2/opencv.hpp>
#include "ModelFitting/Engine/DataVsModelInputTraits.h"

namespace ModelFitting {

template <>
struct DataVsModelInputTraits<cv::Mat> {
  
  using iterator = decltype(std::declval<cv::Mat>().begin<double>());
  
  static iterator begin(cv::Mat& input) {
    return input.begin<double>();
  }
  
  static iterator end(cv::Mat& input) {
    return input.end<double>();
  }
  
  static size_t size(const cv::Mat& input) {
    return input.cols * input.rows;
  }
  
};

} // end of namespace ModelFitting

#endif	/* MODELFITTING_OPENCVDATAVSMODELINPUTTRAITS_H */

