/** 
 * @file utils.h
 * @date September 2, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef UTILS_H
#define	UTILS_H

#include <iostream>
#include <memory>
#include <utility>
#include <valarray>
#include <string>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <CCfits/CCfits>
#include "ElementsKernel/Exception.h"
#include "ModelFitting/Image/OpenCvPsf.h"
#include "ModelFitting/utils.h"

/// Writes an OpenCv Mat to an image FITS file (prepend the filename with '!' to
/// override existing files)
void writeToFits(const cv::Mat& image, const std::string& filename) {
  std::valarray<double> data (image.total());
  std::copy(image.begin<double>(), image.end<double>(), begin(data));
  long naxis = 2;
  long naxes[2] = {image.size[1], image.size[0]};
  std::unique_ptr<CCfits::FITS> pFits {new CCfits::FITS("!"+filename, DOUBLE_IMG, naxis, naxes)};
  pFits->pHDU().write(1, image.total(), data);
  std::cout << "Created file " << filename << '\n';
}

/// Reads a PSF from a fits file. The image must be square and have sides of odd
/// number of pixels. The pixel scale is read by the header keyword SCALE which
/// must be present
ModelFitting::OpenCvPsf readPsf(const std::string& filename) {
  // Read the HDU from the file
  std::unique_ptr<CCfits::FITS> pFits {new CCfits::FITS(filename)};
  auto& image_hdu = pFits->pHDU();
  // Get the pixel scale from the header
  double pixel_scale = 0.;
  image_hdu.readKey("SCALE", pixel_scale);
  // Get the dimension of the image
  if (image_hdu.axis(0) != image_hdu.axis(1)) {
    throw Elements::Exception() << "Non square PSF (" << image_hdu.axis(0) << 'X'
                                << image_hdu.axis(1) << ')';
  }
  auto size = image_hdu.axis(0);
  // Get the data
  std::valarray<double> data {};
  image_hdu.read(data);
  cv::Mat kernel (size, size, CV_64F);
  std::copy(begin(data), end(data), kernel.begin<double>());
  return ModelFitting::OpenCvPsf {pixel_scale, kernel};
}

/// Reads an image from a FITS file. The header must contain the keyword SCALE
/// with the pixelscale of the image. It returns a pair of a cv::Mat containing
/// the data and a double with the pixel scale.
std::pair<cv::Mat, double> readImage(const std::string& filename) {
  // Read the HDU from the file
  std::unique_ptr<CCfits::FITS> pFits {new CCfits::FITS(filename)};
  auto& image_hdu = pFits->pHDU();
  // Get the pixel scale from the header
  double pixel_scale = 0.;
  image_hdu.readKey("SCALE", pixel_scale);
  // Get the dimension of the image
  auto width = image_hdu.axis(1);
  auto height = image_hdu.axis(0);
  // Get the data
  std::valarray<double> data {};
  image_hdu.read(data);
  cv::Mat image (width, height, CV_64F);
  std::copy(begin(data), end(data), image.begin<double>());
  return std::make_pair(image, pixel_scale);
}

// Prints on the screen the info of the levmar minimization
void printLevmarInfo(std::array<double,10> info) {
  std::cout << "\nMinimization info:\n";
  std::cout << "  ||e||_2 at initial p: " << info[0] << '\n';
  std::cout << "  ||e||_2: " << info[1] << '\n';
  std::cout << "  ||J^T e||_inf: " << info[2] << '\n';
  std::cout << "  ||Dp||_2: " << info[3] << '\n';
  std::cout << "  mu/max[J^T J]_ii: " << info[4] << '\n';
  std::cout << "  # iterations: " << info[5] << '\n';
  switch ((int)info[6]) {
  case 1:
    std::cout << "  stopped by small gradient J^T e\n";
    break;
  case 2:
    std::cout << "  stopped by small Dp\n";
    break;
  case 3:
    std::cout << "  stopped by itmax\n";
    break;
  case 4:
    std::cout << "  singular matrix. Restart from current p with increased mu\n";
    break;
  case 5:
    std::cout << "  no further error reduction is possible. Restart with increased mu\n";
    break;
  case 6:
    std::cout << "  stopped by small ||e||_2\n";
    break;
  case 7:
    std::cout << "  stopped by invalid (i.e. NaN or Inf) func values; a user error\n";
    break;
  }
  std::cout << "  # function evaluations: " << info[7] << '\n';
  std::cout << "  # Jacobian evaluations: " << info[8] << '\n';
  std::cout << "  # linear systems solved: " << info[9] << "\n\n";
}

#endif	/* UTILS_H */

