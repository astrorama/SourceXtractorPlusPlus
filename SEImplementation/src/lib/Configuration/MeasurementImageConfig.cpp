/*
 * @file MeasurementImageConfig.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <utility>
#include <limits>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

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

using namespace Euclid::Configuration;
namespace fs = boost::filesystem;

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

  if (images.size() > 0) {
    for (auto& p : images) {
      PyMeasurementImage& py_image = p.second;
      validateImagePaths(py_image);

      logger.debug() << "Initializing MeasurementImageConfig";

      auto flux_scale = py_image.flux_scale;

      m_measurement_images.push_back(createMeasurementImage(py_image));
      m_coordinate_systems.push_back(std::make_shared<WCS>(py_image.file));
      m_psfs_paths.push_back(py_image.psf_file);
      m_gains.push_back(py_image.gain / flux_scale);
      m_saturation_levels.push_back(py_image.saturation * flux_scale);
      m_image_ids.push_back(py_image.id);
      m_paths.push_back(py_image.file);

      logger.info() << "Loaded measurement image: " << py_image.file;
      logger.info() << "\tWeight: " << py_image.weight_file;
      logger.info() << "\tPSF: " << py_image.psf_file;
      logger.info() << "\tGain: " << py_image.gain;
      logger.info() << "\tSaturation: " << py_image.saturation;
      logger.info() << "\tFlux scale: " << py_image.flux_scale;

      m_absolute_weights.push_back(py_image.weight_absolute);
      m_weight_thresholds.push_back(extractWeightThreshold(py_image));

      auto weight_map = createWeightMap(py_image);
      if (weight_map != nullptr && flux_scale != 1. && py_image.weight_absolute) {
        m_weight_images.push_back(MultiplyImage<WeightImage::PixelType>::create(
            weight_map, py_image.flux_scale * py_image.flux_scale));
      } else {
        m_weight_images.push_back(weight_map);
      }
    }
  } else {
    logger.debug() << "No measurement image provided, using the detection image for measurements";

    auto detection_image = getDependency<DetectionImageConfig>();
    auto weight_image = getDependency<WeightImageConfig>();

    // note: flux scale was already applied

    m_measurement_images.push_back(detection_image.getDetectionImage());
    m_coordinate_systems.push_back(detection_image.getCoordinateSystem());
    m_psfs_paths.push_back("");
    m_weight_images.push_back(weight_image.getWeightImage());
    m_absolute_weights.push_back(weight_image.isWeightAbsolute());
    m_weight_thresholds.push_back(weight_image.getWeightThreshold());
    m_gains.push_back(detection_image.getGain());
    m_saturation_levels.push_back(detection_image.getSaturation());
    m_image_ids.push_back(0);
    m_paths.push_back(detection_image.getDetectionImagePath());
  }
}

const std::vector<std::shared_ptr<MeasurementImage>>& MeasurementImageConfig::getMeasurementImages() const {
  return m_measurement_images;
}

const std::vector<int>& MeasurementImageConfig::getImageIds() const {
  return m_image_ids;
}

const std::vector<std::shared_ptr<CoordinateSystem>>& MeasurementImageConfig::getCoordinateSystems() const {
  return m_coordinate_systems;
}

const std::vector<std::shared_ptr<WeightImage>>& MeasurementImageConfig::getWeightImages() const {
  return m_weight_images;
}

const std::vector<bool>& MeasurementImageConfig::getAbsoluteWeights() const {
  return m_absolute_weights;
}

const std::vector<WeightImage::PixelType>& MeasurementImageConfig::getWeightThresholds() const {
  return m_weight_thresholds;
}

const std::vector<std::string> MeasurementImageConfig::getPsfsPaths() const {
  return m_psfs_paths;
}

const std::vector<std::string> MeasurementImageConfig::getImagePaths() const {
  return m_paths;
}

const std::vector<SeFloat>& MeasurementImageConfig::getGains() const {
  return m_gains;
}

const std::vector<MeasurementImage::PixelType>& MeasurementImageConfig::getSaturationLevels() const {
  return m_saturation_levels;
}

} // end of namespace SExtractor
