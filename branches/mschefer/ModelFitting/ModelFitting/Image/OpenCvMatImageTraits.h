/** 
 * @file OpenCvMatImageTraits.h
 * @date August 27, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_OPENCVMATIMAGETRAITS_H
#define	MODELFITTING_OPENCVMATIMAGETRAITS_H

#include <cmath>
#include <algorithm>
#include <chrono>
#include <utility>
#include <opencv2/opencv.hpp>
#include "ModelFitting/Image/ImageTraits.h"

namespace ModelFitting {

template <>
struct ImageTraits<cv::Mat> {
  
  using iterator = decltype(std::declval<cv::Mat>().begin<double>());

  static cv::Mat factory(std::size_t width, std::size_t height) {
    return cv::Mat::zeros((int)height, (int)width, CV_64F);
  }
  
  static std::size_t width(const cv::Mat& image) {
    return image.cols;
  }
  
  static std::size_t height(const cv::Mat& image) {
    return image.rows;
  }
  
  static double& at(cv::Mat& image, std::size_t x, std::size_t y) {
    return image.at<double>((int)y, (int)x);
  }
  
  static iterator begin(cv::Mat& image) {
    return image.begin<double>();
  }
  
  static iterator end(cv::Mat& image) {
    return image.end<double>();
  }
  
  static void addImageToImage(cv::Mat& image1, const cv::Mat& image2,
                              double scale_factor, double x, double y) {
    // Calculate the size in pixels of the image2 after in the scale of image1
    double scaled_width = width(image2) * scale_factor;
    double scaled_height = height(image2) * scale_factor;
    // Calculate the window of the image1 which is affected
    int x_min = std::floor(x - scaled_width / 2.);
    int x_max = std::ceil(x + scaled_width / 2.);
    int window_width = x_max - x_min;
    int y_min = std::floor(y - scaled_height / 2.);
    int y_max= std::ceil(y + scaled_height / 2.);
    int window_height = y_max - y_min;
    // Calculate the shift of the image2 inside the window
    double x_shift = x - scaled_width / 2. - x_min;
    double y_shift = y - scaled_height / 2. - y_min;
    // Create the scaled and shifted window
    cv::Mat affine_trans = (cv::Mat_<double>(2, 3) << 
                               scale_factor, 0., x_shift,
                               0., scale_factor, y_shift
                           );
    auto window = factory(window_width, window_height);
    cv::warpAffine(image2, window, affine_trans, {window_width, window_height}, cv::INTER_LANCZOS4);
    // We need to correct the window for the scaling, so it has the same integral
    // with the image2
    double corr_factor = 1. / (scale_factor * scale_factor);
    window = corr_factor * window;
    // Add the window to the image1
    for(int x_im=std::max(x_min,0); x_im<std::min(x_max,image1.cols); ++x_im) {
      for (int y_im=std::max(y_min,0); y_im<std::min(y_max,image1.rows); ++y_im) {
        int x_win = x_im - x_min;
        int y_win = y_im - y_min;
        at(image1, x_im, y_im) += at(window, x_win, y_win);
      }
    }
  }
  
}; // end of class ImageTraits<cv::Mat>

} // end of namespace ModelFitting

#endif	/* MODELFITTING_OPENCVMATIMAGETRAITS_H */

