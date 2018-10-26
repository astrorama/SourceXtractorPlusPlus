/* 
 * @file MeasurementImage.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _PYTHONCONFIG_PYMEASUREMENTIMAGE_H
#define _PYTHONCONFIG_PYMEASUREMENTIMAGE_H

#include <string>
#include <PythonConfig/PyId.h>

namespace SExtractor {

class PyMeasurementImage : public PyId {
  
public:
  PyMeasurementImage(std::string file, std::string psf_file, std::string weight_file);

  std::string file;
  double gain;
  double saturation;
  double flux_scale;
  std::string psf_file;
  std::string weight_file;
  std::string weight_type;
  bool weight_absolute;
  double weight_scaling;
  bool has_weight_threshold;
  double weight_threshold;
  
};

}

#endif // _PYTHONCONFIG_PYMEASUREMENTIMAGE_H

