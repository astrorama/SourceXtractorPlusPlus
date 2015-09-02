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

