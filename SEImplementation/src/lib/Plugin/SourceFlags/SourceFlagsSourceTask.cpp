/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General
 * Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file SourceFlagsSourceTask.cpp
 *
 * @date Jul 12, 2018
 * @author mkuemmel@usm.lmu.de
 */

#include "SEImplementation/Plugin/SourceFlags/SourceFlagsSourceTask.h"
#include "SEImplementation/Plugin/SourceFlags/SourceFlags.h"

namespace SExtractor {

void SourceFlagsSourceTask::computeProperties(SourceInterface &source) const {
  std::vector<Flags> source_flags;

  for (auto group : m_instances_per_group) {
    Flags source_flag = Flags::NONE;

    for (auto instance : group.second) {
      // add the saturate flag as "4"
      source_flag |= Flags::SATURATED * source.getProperty<SaturateFlag>(instance).getSaturateFlag();

      // add the boundary flag as "8"
      source_flag |= Flags::BOUNDARY * source.getProperty<BoundaryFlag>(instance).getBoundaryFlag();
    }

    source_flags.emplace_back(source_flag);
  }

  // set the combined source flag
  source.setProperty<SourceFlags>(source_flags);
};

} // end SExtractor
