/* 
 * @file MeasurementImage.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <PythonConfig/MeasurementImage.h>

namespace SExtractor {

namespace {

int next_image_id = 0;

}

MeasurementImage::MeasurementImage(std::string file, std::string psf_file, std::string weight_file)
        : file(file), psf_file(psf_file), weight_file(weight_file) {
  id = ++next_image_id;
}

}