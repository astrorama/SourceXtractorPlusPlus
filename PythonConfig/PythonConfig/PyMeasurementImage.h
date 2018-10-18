/* 
 * @file MeasurementImage.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _PYTHONCONFIG_PYMEASUREMENTIMAGE_H
#define _PYTHONCONFIG_PYMEASUREMENTIMAGE_H

#include <string>

namespace SExtractor {

class PyMeasurementImage {
  
public:
  PyMeasurementImage(std::string file, std::string psf_file, std::string weight_file);

  int id;
  std::string file;
  double gain;
  double saturation;
  double flux_scale;
  std::string psf_file;
  std::string weight_file;
  
};

}

#endif // _PYTHONCONFIG_PYMEASUREMENTIMAGE_H

