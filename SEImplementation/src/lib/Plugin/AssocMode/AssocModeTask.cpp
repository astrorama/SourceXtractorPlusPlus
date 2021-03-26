/*
 * AssocModeTask.cpp
 *
 *  Created on: Mar 5, 2021
 *      Author: mschefer
 */

#include <iostream>
#include <algorithm>
#include <functional>

#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"

#include "SEImplementation/Plugin/AssocMode/AssocMode.h"
#include "SEImplementation/Plugin/AssocMode/AssocModeTask.h"

namespace SourceXtractor {

//////////////////////////////////////////////////////////////////////////////////////////


namespace {

AssocModeConfig::CatalogEntry getAssocEntryUnknownImpl(const std::vector<AssocModeConfig::CatalogEntry>&) {
  throw Elements::Exception() << "UNKNOWN association mode";
}

AssocModeConfig::CatalogEntry getAssocEntryFirstImpl(const std::vector<AssocModeConfig::CatalogEntry>& entries) {
  return entries.front();
}

AssocModeConfig::CatalogEntry getAssocEntryNearestImpl(const std::vector<AssocModeConfig::CatalogEntry>& entries,
                                                       double x, double y) {
  return *std::min_element(entries.begin(), entries.end(),
      [x, y](const AssocModeConfig::CatalogEntry &a, const AssocModeConfig::CatalogEntry &b) -> bool {
          auto dx_a = a.coord.m_x - x;
          auto dy_a = a.coord.m_y - y;
          auto dx_b = b.coord.m_x - x;
          auto dy_b = b.coord.m_y - y;
          return dx_a*dx_a + dy_a*dy_a < dx_b*dx_b + dy_b*dy_b;
      });
}

AssocModeConfig::CatalogEntry getAssocEntryMinImpl(const std::vector<AssocModeConfig::CatalogEntry>& entries) {
  return *std::min_element(entries.begin(), entries.end(),
      [](const AssocModeConfig::CatalogEntry &a, const AssocModeConfig::CatalogEntry &b) -> bool {
          return a.weight < b.weight;
      });
}

AssocModeConfig::CatalogEntry getAssocEntryMaxImpl(const std::vector<AssocModeConfig::CatalogEntry>& entries) {
  return *std::max_element(entries.begin(), entries.end(),
      [](const AssocModeConfig::CatalogEntry &a, const AssocModeConfig::CatalogEntry &b) -> bool {
          return a.weight < b.weight;
      });
}

AssocModeConfig::CatalogEntry getAssocEntryMeanImpl(const std::vector<AssocModeConfig::CatalogEntry>& entries) {
  AssocModeConfig::CatalogEntry result;
  result.assoc_columns.resize(entries.front().assoc_columns.size());
  double total_weight = 0.0;
  for (auto& entry : entries) {
    total_weight += entry.weight;
    for (size_t i=0; i < result.assoc_columns.size(); i++) {
      result.assoc_columns[i] += entry.assoc_columns[i] * entry.weight;
    }
  }

  for (size_t i=0; i < result.assoc_columns.size(); i++) {
    result.assoc_columns[i] /= total_weight;
  }

  return result;
}

AssocModeConfig::CatalogEntry getAssocEntrySumImpl(const std::vector<AssocModeConfig::CatalogEntry>& entries) {
  AssocModeConfig::CatalogEntry result;
  result.assoc_columns.resize(entries.front().assoc_columns.size());
  for (auto& entry : entries) {
    for (size_t i=0; i < result.assoc_columns.size(); i++) {
      result.assoc_columns[i] += entry.assoc_columns[i];
    }
  }
  return result;
}

AssocModeConfig::CatalogEntry getAssocEntryMagMeanImpl(const std::vector<AssocModeConfig::CatalogEntry>& entries) {
  AssocModeConfig::CatalogEntry result;
  result.assoc_columns.resize(entries.front().assoc_columns.size());
  double total_weight = 0.0;
  for (auto& entry : entries) {
    double linear_weight = pow(10.0, -0.4 * entry.weight);
    total_weight += linear_weight;
    for (size_t i=0; i < result.assoc_columns.size(); i++) {
      result.assoc_columns[i] += entry.assoc_columns[i] * linear_weight;
    }
  }

  for (size_t i=0; i < result.assoc_columns.size(); i++) {
    result.assoc_columns[i] /= total_weight;
  }

  return result;
}

AssocModeConfig::CatalogEntry getAssocEntryMagSumImpl(const std::vector<AssocModeConfig::CatalogEntry>& entries) {
  AssocModeConfig::CatalogEntry result;
  result.assoc_columns.resize(entries.front().assoc_columns.size());
  for (auto& entry : entries) {
    for (size_t i=0; i < result.assoc_columns.size(); i++) {
      result.assoc_columns[i] += pow(10.0, -0.4 * entry.assoc_columns[i]);
    }
  }

  for (size_t i=0; i < result.assoc_columns.size(); i++) {
    result.assoc_columns[i] = -2.5 * log10(result.assoc_columns[i]);
  }

  return result;
}

}

AssocModeTask::AssocModeTask(const std::vector<AssocModeConfig::CatalogEntry>& catalog,
                             AssocModeConfig::AssocMode assoc_mode, double radius) :
                             m_catalog(catalog), m_assoc_mode(assoc_mode), m_radius(radius) {}

void AssocModeTask::computeProperties(SourceInterface &source) const {
  using namespace std::placeholders;  // for _1, _2, _3...

  // get the object center
  const auto& x = source.getProperty<PixelCentroid>().getCentroidX();
  const auto& y = source.getProperty<PixelCentroid>().getCentroidY();

  auto nearby_catalog_entries = findSourcesWithinRadius(x, y, m_radius);

  if (nearby_catalog_entries.size() == 0) {
    // No match
    source.setProperty<AssocMode>(false, std::vector<double>({99}));
  } else {
    const std::map<AssocModeConfig::AssocMode, AssocModeTask::GetAssocResult> assoc_mode_implementation_table {
      std::make_pair(AssocModeConfig::AssocMode::UNKNOWN, getAssocEntryUnknownImpl),
      std::make_pair(AssocModeConfig::AssocMode::FIRST, getAssocEntryFirstImpl),
      std::make_pair(AssocModeConfig::AssocMode::NEAREST, std::bind(getAssocEntryNearestImpl, _1, x, y)),
      std::make_pair(AssocModeConfig::AssocMode::MIN, getAssocEntryMinImpl),
      std::make_pair(AssocModeConfig::AssocMode::MAX, getAssocEntryMaxImpl),
      std::make_pair(AssocModeConfig::AssocMode::MEAN, getAssocEntryMeanImpl),
      std::make_pair(AssocModeConfig::AssocMode::SUM, getAssocEntrySumImpl),
      std::make_pair(AssocModeConfig::AssocMode::MAG_SUM, getAssocEntryMagSumImpl),
      std::make_pair(AssocModeConfig::AssocMode::MAG_MEAN, getAssocEntryMagMeanImpl)
    };

    auto assoc_data = assoc_mode_implementation_table.at(m_assoc_mode)(nearby_catalog_entries);
    source.setProperty<AssocMode>(true, assoc_data.assoc_columns);
  }
}

std::vector<AssocModeConfig::CatalogEntry> AssocModeTask::findSourcesWithinRadius(double x, double y, double radius) const {
  std::vector<AssocModeConfig::CatalogEntry> selection;
  for (auto& entry : m_catalog) {
    auto dx = entry.coord.m_x - x;
    auto dy = entry.coord.m_y - y;
    if (dx*dx + dy*dy < radius*radius) {
      selection.emplace_back(entry);
    }
  }
  return selection;
}

}
