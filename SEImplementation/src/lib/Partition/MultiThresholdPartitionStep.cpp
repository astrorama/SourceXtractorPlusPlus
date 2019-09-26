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
 * MultiThresholdPartitionStep.cpp
 *
 *  Created on: Jan 17, 2017
 *      Author: mschefer
 */

#include "SEImplementation/Partition/MultiThresholdPartitionStep.h"

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/SubtractImage.h"

#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/PixelBoundaries/PixelBoundaries.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"

#include "SEImplementation/Property/SourceId.h"

#include "SEImplementation/Segmentation/Lutz.h"

namespace SExtractor {

class MultiThresholdNode : public std::enable_shared_from_this<MultiThresholdNode> {
public:

  MultiThresholdNode(const std::vector<PixelCoordinate>& pixel_list, SeFloat threshold)
    : m_pixel_list(pixel_list), m_is_split(false), m_threshold(threshold) {
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
      total_intensity += (image.getValue(pixel_coord) - m_threshold);
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

  void addPixel(PixelCoordinate pixel) {
    m_pixel_list.emplace_back(pixel);
  }

  SeFloat getThreshold() const {
    return m_threshold;
  }

private:
  std::vector<PixelCoordinate> m_pixel_list;

  std::weak_ptr<MultiThresholdNode> m_parent;
  std::vector<std::shared_ptr<MultiThresholdNode>> m_children;

  bool m_is_split;

  SeFloat m_threshold;
};

std::vector<std::shared_ptr<SourceInterface>> MultiThresholdPartitionStep::partition(
    std::shared_ptr<SourceInterface> original_source) const {

  auto parent_source_id = original_source->getProperty<SourceId>().getSourceId();

  auto& detection_frame = original_source->getProperty<DetectionFrame>();
  const auto labelling_image = detection_frame.getFrame()->getFilteredImage();

  auto& pixel_boundaries = original_source->getProperty<PixelBoundaries>();

  auto& pixel_coords = original_source->getProperty<PixelCoordinateList>().getCoordinateList();
  auto image = VectorImage<DetectionImage::PixelType>::create(
      pixel_boundaries.getWidth(), pixel_boundaries.getHeight(), pixel_boundaries.getMin());
  image->fillValue(0);


  auto min_value = original_source->getProperty<PeakValue>().getMinValue() * .8;
  auto peak_value = original_source->getProperty<PeakValue>().getMaxValue();

  for (auto pixel_coord : pixel_coords) {
    auto value = labelling_image->getValue(pixel_coord);
    image->setValue(pixel_coord, value);
  }

  auto root = std::make_shared<MultiThresholdNode>(pixel_coords, 0);

  std::list<std::shared_ptr<MultiThresholdNode>> active_nodes { root };
  std::list<std::shared_ptr<MultiThresholdNode>> junction_nodes;

  // Build the tree
  for (unsigned int i = 1; i < m_thresholds_nb; i++) {

    auto threshold = min_value * pow(peak_value / min_value, (double) i / m_thresholds_nb);
    auto subtracted_image = SubtractImage<DetectionImage::PixelType>::create(image, threshold);

    LutzList lutz;
    lutz.labelImage(*subtracted_image, image->getOffset());

    std::list<std::shared_ptr<MultiThresholdNode>> active_nodes_copy(active_nodes);
    for (auto& node : active_nodes_copy) {
      int nb_of_groups_inside = 0;
      for (auto& pixel_group : lutz.getGroups()) {
        if (pixel_group.pixel_list.size() >= m_min_deblend_area && node->contains(pixel_group)) {
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
          if (pixel_group.pixel_list.size() >= m_min_deblend_area && node->contains(pixel_group)) {
            auto new_node = std::make_shared<MultiThresholdNode>(pixel_group.pixel_list, threshold);
            node->addChild(new_node);
            active_nodes.push_back(new_node);
          }
        }
      }
    }
  }

  // Identify the sources
  double intensity_threshold = root->getTotalIntensity(*image) * m_contrast;

  std::vector<std::shared_ptr<MultiThresholdNode>> source_nodes;
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

  std::vector<std::shared_ptr<SourceInterface>> sources;
  if (source_nodes.empty()) {
    return { original_source }; // no split, just forward the source unchanged
  }

  for (auto source_node : source_nodes) {
    // remove pixels in the new sources from the image
    for (auto& pixel : source_node->getPixels()) {
      image->setValue(pixel, 0);
    }

    auto new_source = m_source_factory->createSource();

    new_source->setProperty<PixelCoordinateList>(source_node->getPixels());
    new_source->setProperty<DetectionFrame>(detection_frame.getFrame());

    sources.push_back(new_source);
  }

  auto new_sources = reassignPixels(sources, pixel_coords, image, source_nodes);

  for (auto& new_source : new_sources) {
    new_source->setProperty<DetectionFrame>(detection_frame.getFrame());
    new_source->setProperty<SourceId>(parent_source_id);
  }

  return new_sources;
}

std::vector<std::shared_ptr<SourceInterface>> MultiThresholdPartitionStep::reassignPixels(
    const std::vector<std::shared_ptr<SourceInterface>>& sources,
    const std::vector<PixelCoordinate>& pixel_coords,
    std::shared_ptr<VectorImage<DetectionImage::PixelType>> image,
    const std::vector<std::shared_ptr<MultiThresholdNode>>& source_nodes
    ) const {

  std::vector<SeFloat> amplitudes;
  for (auto& source : sources) {
    auto& pixel_list = source->getProperty<PixelCoordinateList>().getCoordinateList();
    auto& shape_parameters = source->getProperty<ShapeParameters>();

    auto thresh = source->getProperty<PeakValue>().getMinValue();
    auto peak = source->getProperty<PeakValue>().getMaxValue();

    auto dist = pixel_list.size() / (2.0 * M_PI * shape_parameters.getAbcor() * shape_parameters.getEllipseA() * shape_parameters.getEllipseB());
    auto amp = dist < 70.0 ? thresh * expf(dist) : 4.0 * peak;

    // limit expansion ??
    if (amp > 4.0 * peak) {
      amp = 4.0 * peak;
    }

    amplitudes.push_back(amp);
  }

  for (auto pixel : pixel_coords) {
    if (image->getValue(pixel) > 0) {
      SeFloat cumulated_probability = 0;
      std::vector<SeFloat> probabilities;

      SeFloat min_dist = std::numeric_limits<SeFloat>::max();
      std::shared_ptr<MultiThresholdNode> closest_source_node;

      int i = 0;
      for (auto& source : sources) {
        auto& shape_parameters = source->getProperty<ShapeParameters>();
        auto& pixel_centroid = source->getProperty<PixelCentroid>();

        auto dx = pixel.m_x - pixel_centroid.getCentroidX();
        auto dy = pixel.m_y - pixel_centroid.getCentroidY();

        auto dist = 0.5 * (shape_parameters.getEllipseCxx()*dx*dx +
            shape_parameters.getEllipseCyy()*dy*dy + shape_parameters.getEllipseCxy()*dx*dy) /
            shape_parameters.getAbcor();

        if (dist < min_dist) {
          min_dist = dist;
          closest_source_node = source_nodes[i];
        }

        cumulated_probability += dist < 70.0 ? amplitudes[i] * expf(-dist) : 0.0;

        probabilities.push_back(cumulated_probability);
        i++;
      }

      if (probabilities.back() > 1.0e-31) {
        // TODO probably should use a better RNG
        auto drand = double(probabilities.back()) * double(rand()) / RAND_MAX;

        unsigned int i=0;
        for (; i<probabilities.size() && drand >= probabilities[i]; i++);
        if (i < source_nodes.size()) {
          source_nodes[i]->addPixel(pixel);
        } else {
          std::cout << i << " oops " << drand << " " << probabilities.back() << std::endl;
        }

      } else {
        // select closest source
        closest_source_node->addPixel(pixel);
      }
    }
  }

  int total_pixels = 0;

  std::vector<std::shared_ptr<SourceInterface>> new_sources;
  for (auto source_node : source_nodes) {
    // remove pixels in the new sources from the image
    for (auto& pixel : source_node->getPixels()) {
      image->setValue(pixel, 0);
    }

    auto new_source = m_source_factory->createSource();

    auto pixels = source_node->getPixels();
    total_pixels += pixels.size();

    new_source->setProperty<PixelCoordinateList>(pixels);

    new_sources.push_back(new_source);
  }

  return new_sources;
}


} // namespace


