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
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelEvaluator.h"

#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"

#include "SEImplementation/Deblending/Cleaning.h"

namespace SourceXtractor {

inline bool operator<(SourceGroupInterface::iterator a, SourceGroupInterface::iterator b) {
  return &(*a) < &(*b);
}

void Cleaning::deblend(SourceGroupInterface& group) const {
  if (group.size() <= 1) {
    return;
  }

  std::vector<SourceGroupInterface::iterator> sources_to_clean;
  std::vector<SourceGroupInterface::iterator> remaining_sources;

  // iterate through all sources
  for (auto it = group.begin(); it != group.end(); ++it) {
    if (shouldClean(*it, group)) {
      sources_to_clean.push_back(it);
    } else {
      remaining_sources.push_back(it);
    }
  }

  if (sources_to_clean.size() > 0) {
    if (remaining_sources.size() > 1) {
      std::map<SourceGroupInterface::iterator, std::vector<SourceGroupInterface::iterator>> merging_map;
      for (auto it : sources_to_clean) {
        auto influential_source = findMostInfluentialSource(*it, remaining_sources);
        merging_map[influential_source].push_back(it);
      }

      for (auto merging_pair : merging_map) {
        if (merging_pair.second.size() > 0) {
          auto new_source = mergeSources(*merging_pair.first, merging_pair.second);
          group.addSource(new_source);
          group.removeSource(merging_pair.first);
        }
      }
    } else if (remaining_sources.size() == 1) {
      auto new_source = mergeSources(*remaining_sources[0], sources_to_clean);
      group.addSource(new_source);
      group.removeSource(remaining_sources[0]);
    }

    for (auto& it : sources_to_clean) {
      group.removeSource(it);
    }
  }
}

bool Cleaning::shouldClean(SourceInterface& source, SourceGroupInterface& group) const {
  const auto& pixel_list = source.getProperty<PixelCoordinateList>().getCoordinateList();

  std::vector<double> group_influence(pixel_list.size());

  // iterate through all other sources in the group
  for (auto it = group.begin(); it != group.end(); ++it) {
    if (&(*it) == &source) { // skip self
      continue;
    }

    auto &model = it->getProperty<MoffatModelEvaluator>();
    int i = 0;
    for (auto pixel : pixel_list) {
      auto pixel_value = model.getValue(pixel.m_x, pixel.m_y);
      group_influence[i++] += pixel_value;
    }
  }

  unsigned int still_valid_pixels = 0;
  const auto& pixel_values = source.getProperty<DetectionFramePixelValues>().getFilteredValues();
  int i = 0;
  for (auto value : pixel_values) {
    if (value > group_influence[i++]) {
      still_valid_pixels++;
    }
  }

  return still_valid_pixels < m_min_area;
}

SourceGroupInterface::iterator Cleaning::findMostInfluentialSource(
    SourceInterface& source, const std::vector<SourceGroupInterface::iterator>& candidates) const {

  const auto& pixel_list = source.getProperty<PixelCoordinateList>().getCoordinateList();

  std::vector<double> total_influence_of_sources(candidates.size());

  // iterate through all other sources in the group
  for (size_t i = 0; i < candidates.size(); i++) {
    auto &model = candidates[i]->getProperty<MoffatModelEvaluator>();
    for (auto pixel : pixel_list) {
      auto pixel_value = model.getValue(pixel.m_x, pixel.m_y);
      total_influence_of_sources[i] += pixel_value;
    }
  }

  SourceGroupInterface::iterator most_influential_source(candidates[0]);
  double most_influential_source_value = 0;
  for (size_t i = 0; i < candidates.size(); i++) {
    if (total_influence_of_sources[i] >= most_influential_source_value) {
      most_influential_source = candidates[i];
      most_influential_source_value = total_influence_of_sources[i];
    }
  }
  return most_influential_source;
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
  new_source->setProperty<DetectionFrame>(parent.getProperty<DetectionFrame>().getEncapsulatedFrame());
  new_source->setProperty<SourceId>(parent.getProperty<SourceId>().getSourceId());

  return new_source;
}

std::set<PropertyId> Cleaning::requiredProperties() const {
  return {
    PropertyId::create<PixelCoordinateList>(),
    PropertyId::create<MoffatModelEvaluator>()
  };
}

}

