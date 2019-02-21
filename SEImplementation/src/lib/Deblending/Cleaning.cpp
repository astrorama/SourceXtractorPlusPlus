/*
 * Cleaning.cpp
 *
 *  Created on: 2018 M12 18
 *      Author: mschefer
 */

#include <vector>
#include <set>
#include <tuple>

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Property/SourceId.h"
#include "SEImplementation/Property/PixelCoordinateList.h"

#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFitting.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFittingUtils.h"

#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"

#include "SEImplementation/Deblending/Cleaning.h"

namespace SExtractor {

inline bool operator<(std::reference_wrapper<SourceInterface> a, std::reference_wrapper<SourceInterface> b) {
  return &a.get() < &b.get();
}

class CleaningTree : public std::enable_shared_from_this<CleaningTree> {
public:
  CleaningTree(SourceGroupInterface::iterator source)
    : m_source(source) {}

  void addChild(std::shared_ptr<CleaningTree> child) {
    m_children.emplace_back(child);
    child->m_parent = shared_from_this();
  }

  bool hasParent() const {
    auto parent = m_parent.lock();
    return parent != nullptr;
  }

  std::vector<SourceGroupInterface::iterator> getAllDescendants() const {
    std::vector<SourceGroupInterface::iterator> descendants;
    for (auto& child : m_children) {
      descendants.push_back(child->m_source);
      auto descendants_of_children = child->getAllDescendants();
      descendants.insert(descendants.end(), descendants_of_children.begin(), descendants_of_children.end());
    }
    return descendants;
  }

  SourceGroupInterface::iterator getSourceIterator() const {
    return m_source;
  }

private:
  SourceGroupInterface::iterator m_source;
  std::weak_ptr<CleaningTree> m_parent;
  std::list<std::shared_ptr<CleaningTree>> m_children;
};

void Cleaning::deblend(SourceGroupInterface& group) const {
  if (group.size() <= 1) {
    return;
  }

//  for (auto it = group.begin(); it != group.end(); ) {
//    auto parent_source = findParentSource(*it, group);
//    if (parent_source != group.end()) {
//      it = group.removeSource(it);
//    } else {
//      ++it;
//    }
//  }

  std::map<std::reference_wrapper<SourceInterface>, std::shared_ptr<CleaningTree>> cleaning_tree_map;
  for (auto it = group.begin(); it != group.end(); ++it) {
    auto parent_source = findParentSource(*it, group);
    if (parent_source != group.end()) {
      // Add parent_source to the tree_map if it doesn't exist
      if (cleaning_tree_map[*parent_source] == nullptr) {
        cleaning_tree_map[*parent_source] = std::make_shared<CleaningTree>(parent_source);
      }

      // Add current source to the tree_map if it doesn't exist
      if (cleaning_tree_map[*it] == nullptr) {
        cleaning_tree_map[*it] = std::make_shared<CleaningTree>(it);
      }

      // make the source a child of the parent
      cleaning_tree_map[*parent_source]->addChild(cleaning_tree_map[*it]);
    }
  }

  // iterate through top level nodes
  for (auto& cleaning_tree_node : cleaning_tree_map) {
    if (cleaning_tree_node.second->hasParent()) {
      continue;
    }

    auto descendants = cleaning_tree_node.second->getAllDescendants();
    auto merged_source = mergeSources(cleaning_tree_node.first, descendants);

    // remove all sources that were cleaned
    for (auto& descendant : descendants) {
      group.removeSource(descendant);
    }

    // remove the parent and replace it with the new merged source
    group.removeSource(cleaning_tree_node.second->getSourceIterator());
    group.addSource(merged_source);
  }
}

SourceGroupInterface::iterator Cleaning::findParentSource(SourceInterface& source, SourceGroupInterface& group) const {
  const auto& pixel_list = source.getProperty<PixelCoordinateList>().getCoordinateList();

  std::vector<double> group_influence(pixel_list.size());
  std::vector<double> total_influence_of_sources(group.size());

  // iterate through all other sources in the group
  int source_nb = 0;
  for (auto it = group.begin(); it != group.end(); ++it) {
    if (&(*it) == &source) { // skip self
      continue;
    }

    MoffatModelEvaluator model(*it);
    int i = 0;
    for (auto pixel : pixel_list) {
      auto pixel_value = model.getValue(pixel.m_x, pixel.m_y);
      group_influence[i++] += pixel_value;
      total_influence_of_sources[source_nb] += pixel_value;
    }

    source_nb++;
  }

  unsigned int still_valid_pixels = 0;
  const auto& pixel_values = source.getProperty<DetectionFramePixelValues>().getFilteredValues();
  int i = 0;
  for (auto value : pixel_values) {
    if (value > group_influence[i++]) {
      still_valid_pixels++;
    }
  }

  std::cout << still_valid_pixels << " / " << m_min_area << "\n";

  if (still_valid_pixels < m_min_area) {
    SourceGroupInterface::iterator most_influential_source = group.end();
    double most_influential_source_value = 0;
    int source_nb = 0;
    for (auto it = group.begin(); it != group.end(); ++it) {
      if (&(*it) == &source) { // skip self
        continue;
      }
      if (total_influence_of_sources[source_nb] >= most_influential_source_value) {
        most_influential_source = it;
        most_influential_source_value = total_influence_of_sources[source_nb];
      }
      source_nb++;
    }
    return most_influential_source;
  } else {
    return group.end();
  }
}

std::shared_ptr<SourceInterface> Cleaning::mergeSources(SourceInterface& parent,
    const std::vector<SourceGroupInterface::iterator> children) const {

  // Start with a copy of the pixel list of the parent
  auto pixel_list = parent.getProperty<PixelCoordinateList>().getCoordinateList();

  // Merge the pixel lists of all the child sources
  for (const auto& child : children) {
    const auto& pixel_list_to_merge = child->getProperty<PixelCoordinateList>().getCoordinateList();
    pixel_list.insert(pixel_list.end(), pixel_list_to_merge.begin(), pixel_list_to_merge.end());
  }

  // Create a new source with the minimum necessary properties
  auto new_source = m_source_factory->createSource();
  new_source->setProperty<PixelCoordinateList>(pixel_list);
  new_source->setProperty<DetectionFrame>(parent.getProperty<DetectionFrame>().getFrame());
  new_source->setProperty<SourceId>(parent.getProperty<SourceId>().getSourceId());

  return new_source;
}

}

