/*
 * @file MeasurementImageConfig.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <utility>
#include <limits>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/python.hpp>

#include <ElementsKernel/Logging.h>

#include <SEFramework/Image/FitsImageSource.h>
#include <SEFramework/Image/BufferedImage.h>
#include <SEFramework/Image/FitsReader.h>
#include <SEFramework/Image/MultiplyImage.h>

#include <SEImplementation/CoordinateSystem/WCS.h>
#include <SEImplementation/Configuration/WeightImageConfig.h>
#include <SEImplementation/Configuration/PythonConfig.h>
#include <SEImplementation/Configuration/DetectionImageConfig.h>
#include <SEImplementation/PythonConfig/PyMeasurementImage.h>
#include <SEImplementation/Configuration/MeasurementImageConfig.h>
#include <SEUtils/Python.h>
#include <boost/tokenizer.hpp>

using namespace Euclid::Configuration;
namespace fs = boost::filesystem;
namespace py = boost::python;

namespace SExtractor {

MeasurementImageConfig::MeasurementImageConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PythonConfig>();
  declareDependency<WeightImageConfig>();
  declareDependency<DetectionImageConfig>();
}

namespace {

Elements::Logging logger = Elements::Logging::getLogger("Config");

std::map<std::string, WeightImageConfig::WeightType> weight_type_map {
  {"BACKGROUND", WeightImageConfig::WeightType::WEIGHT_TYPE_FROM_BACKGROUND},
  {"RMS", WeightImageConfig::WeightType::WEIGHT_TYPE_RMS},
  {"VARIANCE", WeightImageConfig::WeightType::WEIGHT_TYPE_VARIANCE},
  {"WEIGHT", WeightImageConfig::WeightType::WEIGHT_TYPE_WEIGHT}
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

std::shared_ptr<MeasurementImage> createMeasurementImage(const PyMeasurementImage& py_image) {
  auto fits_image_source = std::make_shared<FitsImageSource<DetectionImage::PixelType>>(py_image.file);
  std::shared_ptr<MeasurementImage> image = BufferedImage<DetectionImage::PixelType>::create(fits_image_source);
  if (py_image.flux_scale != 1.) {
    image = MultiplyImage<MeasurementImage::PixelType>::create(image, py_image.flux_scale);
  }
  return image;
}

std::shared_ptr<WeightImage> createWeightMap(const PyMeasurementImage& py_image) {
  if (py_image.weight_file == "") {
    return nullptr;
  }
  std::shared_ptr<WeightImage> weight_map = FitsReader<WeightImage::PixelType>::readFile(py_image.weight_file);
  auto weight_type_name = boost::to_upper_copy(py_image.weight_type);
  if (weight_type_map.find(weight_type_name) == weight_type_map.end()) {
    throw Elements::Exception() << "Unknown weight map type : " << py_image.weight_type;
  }
  logger.debug() << "w: " << weight_map->getWidth() << " h: " << weight_map->getHeight()
      << " t: " << py_image.weight_type << " s: " << py_image.weight_scaling;
  weight_map = WeightImageConfig::convertWeightMap(weight_map, weight_type_map[weight_type_name], py_image.weight_scaling);
  // Sanity checks
  if (py_image.weight_file != "" && weight_type_map[weight_type_name] == WeightImageConfig::WeightType::WEIGHT_TYPE_FROM_BACKGROUND)
    throw Elements::Exception() << "Please give an appropriate weight type for image: " << py_image.weight_file;
  if (py_image.weight_absolute && py_image.weight_file == "")
    throw Elements::Exception() << "Setting absolute weight but providing *no* weight image does not make sense.";
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
        threshold = threshold;
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

      logger.debug() << "Loading measurement image: " << py_image.file;
      logger.debug() << "\tWeight: " << py_image.weight_file;
      logger.debug() << "\tPSF: " << py_image.psf_file;
      logger.debug() << "\tGain: " << py_image.gain;
      logger.debug() << "\tSaturation: " << py_image.saturation;
      logger.debug() << "\tFlux scale: " << py_image.flux_scale;

      auto flux_scale = py_image.flux_scale;

      MeasurementImageInfo info;

      info.m_path = py_image.file;
      info.m_psf_path = py_image.psf_file;

      info.m_measurement_image = createMeasurementImage(py_image);
      info.m_coordinate_system = std::make_shared<WCS>(py_image.file);

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

      weight_image.isWeightAbsolute(),
      weight_image.getWeightThreshold(),
      (SeFloat) detection_image.getGain(),
      (SeFloat) detection_image.getSaturation(),

      false,
      0.0,

      0 // id
    });


  }
}

} // end of namespace SExtractor
