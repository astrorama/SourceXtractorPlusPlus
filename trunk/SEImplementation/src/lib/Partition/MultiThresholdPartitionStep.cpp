/*
 * MultiThresholdPartitionStep.cpp
 *
 *  Created on: Jan 17, 2017
 *      Author: mschefer
 */

#include <iostream>

#include "SEImplementation/Partition/MultiThresholdPartitionStep.h"

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/SubtractImage.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"

#include "SEImplementation/Segmentation/Lutz.h"

namespace SExtractor {

class MultiThresholdNode : public std::enable_shared_from_this<MultiThresholdNode> {
public:

  MultiThresholdNode(const std::vector<PixelCoordinate>& pixel_list) : m_pixel_list(pixel_list), m_is_split(false) {
  }

  void addChild(std::shared_ptr<MultiThresholdNode> child) {
    m_children.push_back(child);
    child->m_parent = shared_from_this();
  }

  bool contains(const Lutz::PixelGroup& pixel_group) const {
    for (auto pixel : m_pixel_list) {
      if (pixel_group.pixel_list[0] == pixel) {
        return true;
      }
    }
    return false;
  }

  const std::vector<std::shared_ptr<MultiThresholdNode>>& getChildren() const {
    return m_children;
  }

  std::shared_ptr<MultiThresholdNode> getParent() const {
    return m_parent.lock();
  }

  double getTotalIntensity(DetectionImage& image) const {
    DetectionImage::PixelType total_intensity = 0;
    for (const auto& pixel_coord : m_pixel_list) {
      total_intensity += image.getValue(pixel_coord);
    }

    return total_intensity;
  }

  bool isSplit() const {
    return m_is_split;
  }

  void flagAsSplit() {
    m_is_split = true;
    auto parent = m_parent.lock();
    if (parent != nullptr) {
      parent->flagAsSplit();
    }
  }

  const std::vector<PixelCoordinate>& getPixels() const {
    return m_pixel_list;
  }

  void debugPrint() const {
    std::cout << "(" << m_pixel_list.size();

    for (auto& child : m_children) {
      std::cout << ", ";
      child->debugPrint();
    }

    std::cout << ")";
  }

private:
  std::vector<PixelCoordinate> m_pixel_list;

  std::weak_ptr<MultiThresholdNode> m_parent;
  std::vector<std::shared_ptr<MultiThresholdNode>> m_children;

  bool m_is_split;
};

std::vector<std::shared_ptr<SourceInterface>> MultiThresholdPartitionStep::partition(std::shared_ptr<SourceInterface> source) const {

  auto& detection_frame = source->getProperty<DetectionFrame>();
  const auto labelling_image = detection_frame.getLabellingImage();

  const auto& pixel_boundaries = source->getProperty<PixelBoundaries>();
  auto offset = pixel_boundaries.getMin();

  const auto& pixel_coords = source->getProperty<PixelCoordinateList>().getCoordinateList();
  std::vector<PixelCoordinate> offset_image_coordinates;
  for (auto pixel_coord : pixel_coords) {
    offset_image_coordinates.emplace_back(pixel_coord - offset);
  }

  auto image = std::make_shared<VectorImage<DetectionImage::PixelType>>(
      pixel_boundaries.getWidth(), pixel_boundaries.getHeight());
  image->fillValue(0);

  // FIXME: threshold value as image effect is a problem for multi-threshold?
  DetectionImage::PixelType peak_value = std::numeric_limits<DetectionImage::PixelType>::min();
  DetectionImage::PixelType min_value = std::numeric_limits<DetectionImage::PixelType>::max();
  for (auto pixel_coord : pixel_coords) {
    auto value = labelling_image->getValue(pixel_coord);

    image->setValue(pixel_coord - offset, value);
    peak_value = std::max(peak_value, value);
    min_value = std::min(min_value, value);
  }

  auto root = std::make_shared<MultiThresholdNode>(offset_image_coordinates);

  std::list<std::shared_ptr<MultiThresholdNode>> active_nodes { root };
  std::list<std::shared_ptr<MultiThresholdNode>> junction_nodes;

  // Build the tree
  int thresholds_nb = 10;
  for (int i = 1; i < thresholds_nb; i++) {

    auto threshold = min_value * pow(peak_value / min_value, (double) i / thresholds_nb);
    SubtractImage<DetectionImage::PixelType> subtracted_image(image, threshold);

    LutzList lutz;
    lutz.labelImage(subtracted_image);

    std::list<std::shared_ptr<MultiThresholdNode>> active_nodes_copy(active_nodes);
    for (auto& node : active_nodes_copy) {
      int nb_of_groups_inside = 0;
      for (auto& pixel_group : lutz.getGroups()) {
        if (node->contains(pixel_group)) {
          nb_of_groups_inside++;
        }
      }

      if (nb_of_groups_inside == 0) {
        active_nodes.remove(node);
      }

      if (nb_of_groups_inside > 1) {
        active_nodes.remove(node);
        junction_nodes.push_back(node);
        for (auto& pixel_group : lutz.getGroups()) {
          if (node->contains(pixel_group)) {
            auto new_node = std::make_shared<MultiThresholdNode>(pixel_group.pixel_list);
            node->addChild(new_node);
            active_nodes.push_back(new_node);
          }
        }
      }
    }

    //std::cout << "%% " << threshold << " / " << lutz.getGroups().size() << std::endl;
  }

  //root->debugPrint();
  //std::cout << std::endl;

  // Identify the sources
  double intensity_threshold = root->getTotalIntensity(*image) * 0.005; // FIXME config parameter
  std::list<std::shared_ptr<MultiThresholdNode>> source_nodes;
  while (!junction_nodes.empty()) {
    auto node = junction_nodes.back();
    junction_nodes.pop_back();

    int nb_of_children_above_threshold = 0;

    for (auto child : node->getChildren()) {
      if (child->getTotalIntensity(*image) > intensity_threshold) {
        nb_of_children_above_threshold++;
      }
    }

    if (nb_of_children_above_threshold >= 2) {
      node->flagAsSplit();
      for (auto child : node->getChildren()) {
        if (child->getTotalIntensity(*image) > intensity_threshold && !child->isSplit()) {
          source_nodes.push_back(child);
        }
      }
    }
  }
  //std::cout << "%%%%%%%%: " << source_nodes.size() << std::endl;

  std::vector<std::shared_ptr<SourceInterface>> sources;
  if (source_nodes.empty()) {
    sources.push_back(source); // no split, just forward the source unchanged
  }

  for (auto source_node : source_nodes) {
    auto new_source = m_source_factory->createSource();
    new_source->setProperty<PixelCoordinateList>(source_node->getPixels());
    new_source->setProperty<DetectionFrame>(detection_frame.getDetectionImage(),
        detection_frame.getLabellingImage(), detection_frame.getCoordinateSystem());

    sources.push_back(new_source);
  }

  return sources;
}

}

