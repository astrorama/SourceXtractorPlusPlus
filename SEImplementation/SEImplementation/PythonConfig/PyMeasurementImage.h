/* 
 * @file MeasurementImage.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _SEIMPLEMENTATION_PYMEASUREMENTIMAGE_H
#define _SEIMPLEMENTATION_PYMEASUREMENTIMAGE_H

#include <string>
#include <SEImplementation/PythonConfig/PyId.h>

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

#endif // _SEIMPLEMENTATION_PYMEASUREMENTIMAGE_H

