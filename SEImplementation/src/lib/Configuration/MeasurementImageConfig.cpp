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
/*
 * @file MeasurementImageConfig.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <utility>
#include <limits>

#include <boost/algorithm/string.hpp>
#include <boost/python/extract.hpp>
#include <boost/tokenizer.hpp>

#include <ElementsKernel/Logging.h>

#include <SEUtils/Python.h>

#include <SEFramework/Image/BufferedImage.h>
#include <SEFramework/Image/ProcessedImage.h>
#include <SEFramework/FITS/FitsImageSource.h>

#include <SEFramework/CoordinateSystem/WCS.h>
#include <SEImplementation/Configuration/WeightImageConfig.h>
#include <SEImplementation/Configuration/PythonConfig.h>
#include <SEImplementation/Configuration/DetectionImageConfig.h>
#include <SEImplementation/PythonConfig/PyMeasurementImage.h>

#include <SEImplementation/Configuration/MeasurementImageConfig.h>

using namespace Euclid::Configuration;
namespace fs = boost::filesystem;
namespace py = boost::python;

namespace SourceXtractor {

MeasurementImageConfig::MeasurementImageConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PythonConfig>();
  declareDependency<WeightImageConfig>();
  declareDependency<DetectionImageConfig>();
}

namespace {

Elements::Logging logger = Elements::Logging::getLogger("Config");

std::map<std::string, WeightImageConfig::WeightType> weight_type_map {
  {"NONE", WeightImageConfig::WeightType::WEIGHT_TYPE_NONE},
  {"BACKGROUND", WeightImageConfig::WeightType::WEIGHT_TYPE_FROM_BACKGROUND},
  {"RMS", WeightImageConfig::WeightType::WEIGHT_TYPE_RMS},
  {"VARIANCE", WeightImageConfig::WeightType::WEIGHT_TYPE_VARIANCE},
  {"WEIGHT", WeightImageConfig::WeightType::WEIGHT_TYPE_WEIGHT},
  {"UNDEF", WeightImageConfig::WeightType::WEIGHT_TYPE_NONE}
};

void validateImagePaths(const PyMeasurementImage& image) {
  if (!fs::exists(image.file)) {
    throw Elements::Exception() << "File " << image.file << " does not exist";
  }
  if (image.weight_file != "" && !fs::exists(image.weight_file)) {
    throw Elements::Exception() << "File " << image.weight_file << " does not exist";
  }
  if (image.psf_file != "" && !fs::exists(image.psf_file)) {
    throw Elements::Exception() << "File " << image.psf_file << " does not exist";
  }
}

std::shared_ptr<MeasurementImage> createMeasurementImage(
    std::shared_ptr<FitsImageSource> fits_image_source, double flux_scale) {
  std::shared_ptr<MeasurementImage> image = BufferedImage<DetectionImage::PixelType>::create(fits_image_source);
  if (flux_scale != 1.) {
    image = MultiplyImage<MeasurementImage::PixelType>::create(image, flux_scale);
  }
  return image;
}

WeightImageConfig::WeightType getWeightType(const std::string& type_string, const std::string& file_name) {
  // check for a valid weight type
  auto weight_type_name = boost::to_upper_copy(type_string);
  if (weight_type_map.find(weight_type_name) == weight_type_map.end()) {
    throw Elements::Exception() << "Unknown weight map type for measurement weight image " << file_name << ": "<< type_string;
  }

  return weight_type_map[weight_type_name];
}

std::shared_ptr<WeightImage> createWeightMap(const PyMeasurementImage& py_image) {
  auto weight_type = getWeightType(py_image.weight_type, py_image.weight_file);

  // without an image nothing can be done
  if (py_image.weight_file == "") {
    if (weight_type != WeightImageConfig::WeightType::WEIGHT_TYPE_NONE &&
        weight_type != WeightImageConfig::WeightType::WEIGHT_TYPE_FROM_BACKGROUND) {
      throw Elements::Exception() << "Weight type '" << py_image.weight_type << "' is meaningless without a weight image";
    }

    return nullptr;
  }

  if (weight_type == WeightImageConfig::WeightType::WEIGHT_TYPE_NONE ||
      weight_type == WeightImageConfig::WeightType::WEIGHT_TYPE_FROM_BACKGROUND) {
    throw Elements::Exception() << "Please give an appropriate weight type for image: " << py_image.weight_file;
  }

  auto weight_image_source =
      std::make_shared<FitsImageSource>(py_image.weight_file, py_image.weight_hdu, ImageTile::FloatImage);
  std::shared_ptr<WeightImage> weight_map = BufferedImage<WeightImage::PixelType>::create(weight_image_source);

  logger.debug() << "w: " << weight_map->getWidth() << " h: " << weight_map->getHeight()
      << " t: " << py_image.weight_type << " s: " << py_image.weight_scaling;
  weight_map = WeightImageConfig::convertWeightMap(weight_map, weight_type, py_image.weight_scaling);

  return weight_map;
}

WeightImage::PixelType extractWeightThreshold(const PyMeasurementImage& py_image) {
  if (!py_image.has_weight_threshold) {
    return std::numeric_limits<WeightImage::PixelType>::max();
  }
  WeightImage::PixelType threshold = py_image.weight_threshold;
  auto weight_type_name = boost::to_upper_copy(py_image.weight_type);
  switch (weight_type_map[weight_type_name]) {
      default:
      case WeightImageConfig::WeightType::WEIGHT_TYPE_FROM_BACKGROUND:
      case WeightImageConfig::WeightType::WEIGHT_TYPE_RMS:
        threshold = threshold * threshold;
        break;
      case WeightImageConfig::WeightType::WEIGHT_TYPE_VARIANCE:
        break;
      case WeightImageConfig::WeightType::WEIGHT_TYPE_WEIGHT:
        if (threshold>0)
          threshold = 1.0 / threshold;
        else
          threshold = std::numeric_limits<WeightImage::PixelType>::max();
        break; 
  }
  return threshold;
}

}

void MeasurementImageConfig::initialize(const UserValues&) {
  auto images = getDependency<PythonConfig>().getInterpreter().getMeasurementImages();
  auto groups = getDependency<PythonConfig>().getInterpreter().getMeasurementGroups();

  // Delegate into Python to log the measurement configuration
  boost::char_separator<char> line_sep{"\n"};
  for (auto &g : groups) {
    GILStateEnsure ensure;
    std::string group_str = py::extract<std::string>(g.attr("__str__")());
    boost::tokenizer<decltype(line_sep)> tok(group_str, line_sep);
    for (auto &l : tok) {
      logger.info() << l;
    }
  }

  if (images.size() > 0) {
    for (auto& p : images) {
      PyMeasurementImage& py_image = p.second;
      validateImagePaths(py_image);

      logger.debug() << "Loading measurement image: " << py_image.file << " HDU: " << py_image.image_hdu;
      logger.debug() << "\tWeight: " << py_image.weight_file << " HDU: " << py_image.weight_hdu;
      logger.debug() << "\tPSF: " << py_image.psf_file << " HDU: " << py_image.psf_hdu;
      logger.debug() << "\tGain: " << py_image.gain;
      logger.debug() << "\tSaturation: " << py_image.saturation;
      logger.debug() << "\tFlux scale: " << py_image.flux_scale;

      auto flux_scale = py_image.flux_scale;

      MeasurementImageInfo info;

      info.m_path = py_image.file;
      info.m_psf_path = py_image.psf_file;


      auto fits_image_source =
          std::make_shared<FitsImageSource>(py_image.file, py_image.image_hdu, ImageTile::FloatImage);
      info.m_measurement_image = createMeasurementImage(fits_image_source, py_image.flux_scale);
      info.m_coordinate_system = std::make_shared<WCS>(*fits_image_source);

      info.m_gain = py_image.gain / flux_scale;
      info.m_saturation_level = py_image.saturation * flux_scale;
      info.m_id = py_image.id;

      info.m_absolute_weight= py_image.weight_absolute;
      info.m_weight_threshold = extractWeightThreshold(py_image);

      info.m_is_background_constant = py_image.is_background_constant;
      info.m_constant_background_value = py_image.constant_background_value;

      auto weight_map = createWeightMap(py_image);

      if (weight_map != nullptr && flux_scale != 1. && py_image.weight_absolute) {
        info.m_weight_image = MultiplyImage<WeightImage::PixelType>::create(
            weight_map, py_image.flux_scale * py_image.flux_scale);
      } else {
        info.m_weight_image = weight_map;
      }

      info.m_weight_type = getWeightType(py_image.weight_type, py_image.weight_file);

      info.m_image_hdu = py_image.image_hdu;
      info.m_psf_hdu = py_image.psf_hdu;
      info.m_weight_hdu = py_image.weight_hdu;

      m_image_infos.emplace_back(std::move(info));
    }
  } else {
    logger.debug() << "No measurement image provided, using the detection image for measurements";

    auto detection_image = getDependency<DetectionImageConfig>();
    auto weight_image = getDependency<WeightImageConfig>();

    // note: flux scale was already applied

    m_image_infos.emplace_back(MeasurementImageInfo {
      detection_image.getDetectionImagePath(),
      "", // psf path

      detection_image.getDetectionImage(),
      detection_image.getCoordinateSystem(),
      weight_image.getWeightImage(),
      weight_image.getWeightType(),

      weight_image.isWeightAbsolute(),
      weight_image.getWeightThreshold(),
      (SeFloat) detection_image.getGain(),
      (SeFloat) detection_image.getSaturation(),

      false,
      0.0,

      0, // id

      1,1,1 // HDUs
    });


  }
}

} // end of namespace SourceXtractor
