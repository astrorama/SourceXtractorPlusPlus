/* 
 * @file MeasurementImage.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _PYTHONCONFIG_MEASUREMENTIMAGE_H
#define _PYTHONCONFIG_MEASUREMENTIMAGE_H

#include <string>

namespace SExtractor {

class MeasurementImage {
  
public:
  MeasurementImage(std::string file, std::string psf_file, std::string weight_file);

  int id;
  std::string file;
  std::string psf_file;
  std::string weight_file;
  
};

}

#endif // _PYTHONCONFIG_MEASUREMENTIMAGE_H

