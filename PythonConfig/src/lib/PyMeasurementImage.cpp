/* 
 * @file MeasurementImage.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <PythonConfig/PyMeasurementImage.h>

namespace SExtractor {

namespace {

int next_image_id = 0;

}

PyMeasurementImage::PyMeasurementImage(std::string file, std::string psf_file, std::string weight_file)
        : file(file), gain(0.), saturation(0.), flux_scale(1.), psf_file(psf_file), weight_file(weight_file) {
  id = next_image_id;
  ++next_image_id;
}

}