/* 
 * @file MeasurementImageConfig.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <utility>
#include <boost/filesystem.hpp>
#include <ElementsKernel/Logging.h>
#include <SEFramework/Image/FitsImageSource.h>
#include <SEFramework/Image/BufferedImage.h>
#include <SEFramework/Image/FitsReader.h>
#include <SEFramework/Image/MultiplyImage.h>
#include <SEImplementation/CoordinateSystem/WCS.h>
#include <SEImplementation/Configuration/WeightImageConfig.h>
#include <PythonConfig/Configuration/MeasurementImageConfig.h>
#include <PythonConfig/Configuration/PythonConfig.h>
#include <PythonConfig/PyMeasurementImage.h>

using namespace Euclid::Configuration;
namespace fs = boost::filesystem;

namespace SExtractor {

MeasurementImageConfig::MeasurementImageConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PythonConfig>();
}

namespace {

Elements::Logging logger = Elements::Logging::getLogger("Config");

void validateImagePaths(const PyMeasurementImage& image) {
  logger.debug() << "adding: " << image.file << " weight: " << image.weight_file << " psf: " << image.psf_file;
  if (!fs::exists(image.file)) {
    throw Elements::Exception() << "File " << image.file << " does not exist";
  }
  if (image.weight_file != "" && !fs::exists(image.weight_file)) {
    throw Elements::Exception() << "File " << image.weight_file << " does not exist";
  }
  if (!fs::exists(image.psf_file)) {
    throw Elements::Exception() << "File " << image.psf_file << " does not exist";
  }
}

std::shared_ptr<MeasurementImage> createMeasurementImage(const PyMeasurementImage& py_image) {
  auto fits_image_source = std::make_shared<FitsImageSource<DetectionImage::PixelType>>(py_image.file);
  std::shared_ptr<MeasurementImage> image = BufferedImage<DetectionImage::PixelType>::create(fits_image_source);
  logger.debug() << "w: " << image->getWidth() << " h: " << image->getHeight();
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
  std::cout << "w: " << weight_map->getWidth() << " h: " << weight_map->getHeight() << std::endl;
  // TODO: The weight type and scale should be taken from the configuration
  weight_map = WeightImageConfig::convertWeightMap(weight_map, WeightImageConfig::WeightType::WEIGHT_TYPE_RMS);
  return weight_map;
}

}

void MeasurementImageConfig::initialize(const UserValues&) {
  auto images = getDependency<PythonConfig>().getInterpreter().getMeasurementImages();
  
  for (auto& p : images) {
    
    PyMeasurementImage& py_image = p.second;
    validateImagePaths(py_image);
    
    m_measurement_images.push_back(createMeasurementImage(py_image));
    m_coordinate_systems.push_back(std::make_shared<WCS>(py_image.file));
    m_psfs_paths.push_back(py_image.psf_file);
    m_weight_images.push_back(createWeightMap(py_image));
    m_gains.push_back(py_image.gain);
    m_saturation_levels.push_back(py_image.saturation);
  }
}

const std::vector<std::shared_ptr<MeasurementImage>>& MeasurementImageConfig::getMeasurementImages() const {
  return m_measurement_images;
}

const std::vector<std::shared_ptr<CoordinateSystem>>& MeasurementImageConfig::getCoordinateSystems() const {
  return m_coordinate_systems;
}

const std::vector<std::shared_ptr<WeightImage>>& MeasurementImageConfig::getWeightImages() const {
  return m_weight_images;
}

const std::vector<std::string> MeasurementImageConfig::getPsfsPaths() const {
  return m_psfs_paths;
}

const std::vector<SeFloat>& MeasurementImageConfig::getGains() const {
  return m_gains;
}

const std::vector<MeasurementImage::PixelType>& MeasurementImageConfig::getSaturationLevels() const {
  return m_saturation_levels;
}

} // end of namespace SExtractor