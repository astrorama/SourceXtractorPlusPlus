/*
 * CheckImages.h
 *
 *  Created on: May 30, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CHECKIMAGES_CHECKIMAGES_H_
#define _SEIMPLEMENTATION_CHECKIMAGES_CHECKIMAGES_H_

#include "SEFramework/Configuration/Configurable.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/SubtractImage.h"


namespace SExtractor {

class CheckImages : public Configurable {

public:

  virtual ~CheckImages() = default;

  void saveImages();

  std::shared_ptr<VectorImage<DetectionImage::PixelType>> getModelFittingCheckImage() const {
    return m_check_image_model_fitting;
  }

  std::shared_ptr<VectorImage<unsigned int>> getSegmentationImage() const {
    return m_segmentation_image;
  }

  std::shared_ptr<VectorImage<unsigned int>> getPartitionImage() const {
    return m_partition_image;
  }

  void setBackgroundCheckImage(SeFloat background_value, std::shared_ptr<Image<SeFloat>> background_image=nullptr) {
    if (background_image!=nullptr) {
      // if available, copy the pointer
      m_background_image = background_image;
    } else {
      // create a constant image at the given level
      m_background_image = ConstantImage<SeFloat>::create(m_detection_image->getWidth(), m_detection_image->getHeight(), background_value);
    }
  }

  void setVarianceCheckImage(SeFloat variance_value, std::shared_ptr<Image<SeFloat>> variance_image=nullptr) {
    if (variance_image!=nullptr) {
      // if available, copy the pointer
      m_variance_image = variance_image;
    } else {
      // create a constant image at the given level
      m_background_image = ConstantImage<SeFloat>::create(m_detection_image->getWidth(), m_detection_image->getHeight(), variance_value);
    }
  }

  virtual void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;
  virtual void configure(Euclid::Configuration::ConfigManager& manager) override;

  static CheckImages& getInstance() {
    if (m_instance == nullptr) {
      m_instance.reset(new CheckImages);
    }

    return *m_instance;
  }


private:
  CheckImages();

  static std::unique_ptr<CheckImages> m_instance;

  // check image
  std::shared_ptr<VectorImage<DetectionImage::PixelType>> m_check_image_model_fitting;
  std::shared_ptr<DetectionImage> m_detection_image;
  std::shared_ptr<Image<SeFloat>> m_background_image;
  std::shared_ptr<WeightImage> m_variance_image;
  std::shared_ptr<VectorImage<unsigned int>> m_segmentation_image;
  std::shared_ptr<VectorImage<unsigned int>> m_partition_image;

  std::string m_model_fitting_image_filename;
  std::string m_residual_filename;
  std::string m_model_background_filename;
  std::string m_model_variance_filename;
  std::string m_segmentation_filename;
  std::string m_partition_filename;

};

}



#endif /* _SEIMPLEMENTATION_CHECKIMAGES_CHECKIMAGES_H_ */
